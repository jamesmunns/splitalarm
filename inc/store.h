#ifndef _STORE_H
#define _STORE_H

#include "misc.h"
#include "alarm.h"
#include "time.h"

#define STORE_CURRENT_DATA_VERSION  1
#define STORE_CRC_SEED              0xFADBEEF8

typedef struct{
    // VERSION MUST COME FIRST
    uint8_t         stored_version;

    // All other data
    basic_time_t    stored_time;
    alarm_t         stored_alarms[ALARM_CT];

    // Lastly, CRC data
    uint32_t        stored_crc;
}store_data_t;

void store_init(void);
bool store_shadow_load();
bool store_data_get(store_data_t* data);
bool store_data_write( store_data_t* data );

#endif