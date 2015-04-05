#ifndef _CLOCK_H
#define _CLOCK_H

#include "misc.h"

typedef struct
{
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint8_t  pad[3]; // for alignment
    uint32_t year;
} basic_time_t;

typedef enum
{
    INVALID_MONTH,
    JANUARY,
    FEBRURARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER,
    MONTH_COUNT
} basic_month_t;

#define CUR_TIME_PTR NULL

typedef uint8_t day_t;

#define SUNDAY      0x01
#define MONDAY      0x02
#define TUESDAY     0x04
#define WEDNESDAY   0x08
#define THURSDAY    0x10
#define FRIDAY      0x20
#define SATURDAY    0x40

#define ERRY_DAY ( MONDAY | TUESDAY | WEDNESDAY | THURSDAY | FRIDAY | SATURDAY | SUNDAY )
#define WEEKDAYS ( MONDAY | TUESDAY | WEDNESDAY | THURSDAY | FRIDAY )
#define WEEKENDS ( SATURDAY | SUNDAY )


void clock_init(void);
void clock_periodic_second(void);
void clock_cur_time_get(basic_time_t* cur_time);
void clock_string_get( basic_time_t*   gtime,
                       char*           time_str );
void clock_new_time_set(basic_time_t* ntime);
void clock_debug_toggle(bool tog);
bool clock_time_validate(basic_time_t* timecheck);
day_t clock_day_of_week_get( unsigned int  y,
                             unsigned char m,
                             unsigned char d );

#endif