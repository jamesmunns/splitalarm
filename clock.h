#ifndef _CLOCK_H
#define _CLOCK_H

#include "inc/hw_types.h"

typedef struct
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned int  year;
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


void clock_init(void);
void clock_second_tick(void);
void clock_cur_time_get(basic_time_t* cur_time);
void clock_string_get( basic_time_t*   gtime,
                       char*           time_str );


#endif