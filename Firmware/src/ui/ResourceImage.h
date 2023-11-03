#pragma once
#include "AnimatedGIF.h"
#include "DisplayHAL.h"
#include "LittleFS.h"
#include "log.h"
#include <Arduino.h>
#include <string>

struct LittleFile {
  fs::File file;
};

class ResourceImage {
private:
  String filename;
  AnimatedGIF *gif;
  int x;
  int y;
  int width;
  int height;
  ulong nextFrame = 0;

  // we don't own this
  DisplayHAL *currentHal;

public:
  ResourceImage(const String &filename, int x, int y) {
    this->filename = "/" + filename;
    this->x = x;
    this->y = y;

    const char *szFilename = this->filename.c_str();
    gif = new AnimatedGIF();
    gif->begin(BIG_ENDIAN_PIXELS);
    gif->open(szFilename, gifOpen, gifClose, gifRead, gifSeek, gifDraw);
    this->width = gif->getCanvasWidth();
    this->height = gif->getCanvasHeight();
    LV_LOG_INFO(("Created resource image " + this->filename).c_str());
  }

  void tick(GIFDRAW *pDraw) { currentHal->GIFDraw(pDraw, x, y, width, height); }

  void tick(DisplayHAL *displayHal) {
    this->currentHal = displayHal;
    ulong now = millis();

    if (nextFrame < now) {
      int delay = 0;
      gif->playFrame(false, &delay, this);
      nextFrame = now + delay;
    }
  }

  ~ResourceImage() {
    LV_LOG_INFO(("Deleting resource image " + this->filename).c_str());
    gif->close();
    delete gif;
    LV_LOG_INFO(("Deleted resource image " + this->filename).c_str());
  }

  static void *gifOpen(const char *szFilename, int32_t *pFileSize) {
    auto lf = new LittleFile();
    lf->file = LittleFS.open(szFilename, "r");
    *pFileSize = lf->file.size();
    return (void *)lf;
  }

  static void gifClose(void *pHanldle) {
    auto lf = (LittleFile *)pHanldle;
    lf->file.close();
    delete lf;
  }

  static int32_t gifRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    auto lf = (LittleFile *)pFile->fHandle;
    int32_t iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
      iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
      return 0;
    iBytesRead = (int32_t)lf->file.read(pBuf, iBytesRead);
    pFile->iPos = lf->file.position();
    return iBytesRead;
  }

  static int32_t gifSeek(GIFFILE *pFile, int32_t iPosition) {
    auto lf = (LittleFile *)pFile->fHandle;
    lf->file.seek(iPosition);
    pFile->iPos = lf->file.position();
    return pFile->iPos;
  }

  static void gifDraw(GIFDRAW *pDraw) {
    auto img = (ResourceImage *)pDraw->pUser;
    img->tick(pDraw);
  }
};