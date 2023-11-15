#ifndef MOONRAKER_H
#define MOONRAKER_H

#include <WString.h>

// typedef enum {
//     MOONRAKER_STATE_HOMING = 0,
//     MOONRAKER_STATE_PROBING,
//     MOONRAKER_STATE_QGLING,
//     MOONRAKER_STATE_NOZZLE_HEATING,
//     MOONRAKER_STATE_BED_HEATING,
//     MOONRAKER_STATE_PRINTING,
//     MOONRAKER_STATE_IDLE,
// } moonraker_status_t;

typedef struct {
    int16_t bed_actual;
    int16_t bed_target;
    int16_t nozzle_actual;
    int16_t nozzle_target;
    uint8_t progress;
    char file_path[32];

    bool pause;
    bool printing;    // is klipper in a printing task (including printing, pausing, paused, cancelling)
    bool homing;
    bool probing;
    bool qgling;
    bool heating_nozzle;
    bool heating_bed;
} moonraker_data_t;

#define QUEUE_LEN 5
typedef struct {
    String queue[QUEUE_LEN];
    uint8_t index_r;  // Ring buffer read position
    uint8_t index_w;  // Ring buffer write position
    uint8_t count;    // Count of commands in the queue
} post_queue_t;

class MOONRAKER {
    public:
        bool unconnected;   // is KNOMI connected to moonraker
        bool unready; // is moonraker connected to klipper
        bool data_unlock; //
        moonraker_data_t data;
        void http_get_loop(void);
        void http_post_loop(void);
        bool post_to_queue(String path);
        bool post_gcode_to_queue(String gcode);
        String send_request(const char * type, String path);

    private:
        post_queue_t post_queue;
        void get_printer_ready(void);
        void get_printer_info(void);
        void get_progress(void);
        void get_knomi_status(void);
};

extern MOONRAKER moonraker;

#endif
