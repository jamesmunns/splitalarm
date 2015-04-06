#ifndef _ALARM_H
#define _ALARM_H

#include "misc.h"
#include "clock.h"

typedef uint8_t alarm_enabled_flag_t;
#define ALARM_ENABLE    0x01
#define ALARM_TRIGGERED 0x02

typedef enum {
    ALARM_A,
    ALARM_B,
    ALARM_CT
} alarm_id_t;

typedef struct {
    uint8_t              hour;
    uint8_t              minute;
    day_t                day_of_week;
    alarm_enabled_flag_t enable_flags;
} alarm_t;

void alarm_init(void);
void alarm_periodic_second(void);
bool alarm_check_trigger( basic_time_t* cur_time,
                          alarm_t*      alarm );
void alarm_handle_alarm(void);
void alarm_set(alarm_id_t id, alarm_t* alarm);
bool alarm_validate(alarm_t* alarm);
void alarm_acknowledge(void);

#endif