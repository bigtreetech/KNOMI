#include <Arduino.h>
#include "lv_port_fs_littlefs.h"
#include "littlefile.h"

#if LV_LOG_TRACE_FS
#define LV_LOG_TRACEFS(...) LV_LOG_LEVEL_TRACE(__VA_ARGS__)
#else
#define LV_LOG_TRACEFS(...) do {}while(0)
#endif

static void fs_init(void);

static void* fs_open(lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t* drv, void* file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br);
static lv_fs_res_t fs_write(lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t* drv, void* file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p);
static void* fs_dir_open(struct _lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(struct _lv_fs_drv_t * drv, void * rddir_p, char * fn);
static lv_fs_res_t fs_dir_close(struct _lv_fs_drv_t * drv, void * rddir_p);

void lv_port_littlefs_init(void) {
  _lv_fs_init();
  fs_init();

  /*Add a simple drive to open images*/
  static lv_fs_drv_t fs_drv;
  lv_fs_drv_init(&fs_drv);

  fs_drv.letter = 'A';
  fs_drv.open_cb = fs_open;
  fs_drv.close_cb = fs_close;
  fs_drv.read_cb = fs_read;
  fs_drv.write_cb = fs_write;
  fs_drv.seek_cb = fs_seek;
  fs_drv.tell_cb = fs_tell;
  fs_drv.cache_size = 8192;

  fs_drv.dir_close_cb = fs_dir_close;
  fs_drv.dir_open_cb = fs_dir_open;
  fs_drv.dir_read_cb = fs_dir_read;

  lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void) { LittleFS.begin(); }

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void* fs_open(lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode) {
  LV_LOG_TRACEFS(path);
  LV_UNUSED(drv);

  const char* flags = "";

  if (mode == LV_FS_MODE_WR)
    flags = FILE_WRITE;
  else if (mode == LV_FS_MODE_RD)
    flags = FILE_READ;
  else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
    flags = FILE_WRITE;

  File f = LittleFS.open(path, flags);
  if (!f) {
    return NULL;
  }

  LittleFile* lf = new LittleFile{f};

  return (void*)lf;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t* drv, void* file_p) {
  LV_UNUSED(drv);
  LV_LOG_TRACEFS("fs_close");
  LittleFile* lf = (LittleFile*)file_p;

  lf->file.close();

  delete lf;
  return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br) {
  LV_UNUSED(drv);
  LV_LOG_TRACEFS(String(btr).c_str());
  LittleFile* lf = (LittleFile*)file_p;

  *br = lf->file.read((uint8_t*)buf, btr);

  return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw) {
  LV_UNUSED(drv);
  LV_LOG_TRACEFS(String(btw).c_str());
  LittleFile* lf = (LittleFile*)file_p;
  *bw = lf->file.write((uint8_t*)buf, btw);
  return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t* drv, void* file_p, uint32_t pos, lv_fs_whence_t whence) {
  LV_UNUSED(drv);

  LV_LOG_TRACEFS(String(pos).c_str());
  SeekMode mode;
  if (whence == LV_FS_SEEK_SET)
    mode = SeekSet;
  else if (whence == LV_FS_SEEK_CUR)
    mode = SeekCur;
  else if (whence == LV_FS_SEEK_END)
    mode = SeekEnd;

  LittleFile* lf = (LittleFile*)file_p;
  lf->file.seek(pos, mode);
  return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p) {
  LV_UNUSED(drv);
  LV_LOG_TRACEFS("fs_tell");
  LittleFile* lf = (LittleFile*)file_p;
  *pos_p = lf->file.position();
  return LV_FS_RES_OK;
}

static void* fs_dir_open(struct _lv_fs_drv_t * drv, const char * path) {
  LV_UNUSED(drv);
  LV_LOG_INFO("fs_dir_open");
  LV_LOG_INFO(path);
  return nullptr;
}
static lv_fs_res_t fs_dir_read(struct _lv_fs_drv_t * drv, void * rddir_p, char * fn) {
  LV_LOG_INFO("fs_dir_read");
  return LV_FS_RES_NOT_IMP;
}
static lv_fs_res_t fs_dir_close(struct _lv_fs_drv_t * drv, void * rddir_p) {
  LV_LOG_INFO("fs_dir_close");
  return LV_FS_RES_NOT_IMP;
}