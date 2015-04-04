#include "clock.h"
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include <string.h>
#include "cmd.h"

static basic_time_t cur_time;
static unsigned char clock_dbg_flag;
static unsigned char days_per_month[MONTH_COUNT] = {
    00, //Invalid Month
    31, //Jan
    28, //Feb //TODO
    31, //Mar
    30, //Apr
    31, //May
    30, //June
    31, //July
    31, //Aug
    30, //Sep
    31, //Oct
    30, //Nov
    31  //Dec
};

void clock_init(void)
{
    cur_time.year = 2015;
    cur_time.month = 1;
    cur_time.day = 1;
    cur_time.hour = 0;
    cur_time.minute = 0;
    cur_time.second = 0;

    clock_dbg_flag = false;
}

void clock_periodic_second(void)
{
    //Increment, start cascade
    cur_time.second++;

    //Seconds
    if(cur_time.second >= 60)
    {
        cur_time.minute++;
        cur_time.second = 0;
    }

    //Minutes
    if(cur_time.minute >= 60)
    {
        cur_time.hour++;
        cur_time.minute = 0;
    }

    //Hours
    if(cur_time.hour >= 24)
    {
        cur_time.day++;
        cur_time.hour = 0;
    }

    //Days switch on month
    if(cur_time.day > days_per_month[cur_time.month])
    {
        cur_time.month++;
        cur_time.day = 1;
    }

    if(cur_time.month >= MONTH_COUNT)
    {
        cur_time.year++;
        cur_time.month = JANUARY;
    }

    //Debug
    if(clock_dbg_flag)
    {
        char timebuf[32];

        clock_string_get( CUR_TIME_PTR,
                          timebuf );
        cmd_printf("%s\n", timebuf);
    }
}

void clock_cur_time_get(basic_time_t* gtime)
{
    memcpy(&gtime, &cur_time, sizeof(cur_time));
}

void clock_debug_toggle(unsigned char tog)
{
    clock_dbg_flag = tog;
}

//hh:mm:ss dd.mm.yyyy //19
void clock_string_get( basic_time_t*   gtime,
                       char*           time_str )
{
    if(gtime==NULL)
    {
        gtime = &cur_time;
    }

    if(time_str==NULL)
    {
        return;
    }

    usprintf( time_str,
              "%02d:%02d:%02d %02d.%02d.%04d",
              gtime->hour,
              gtime->minute,
              gtime->second,
              gtime->day,
              gtime->month,
              gtime->year );
}

void clock_new_time_set(basic_time_t* ntime)
{
    if(ntime==NULL)
    {
        return;
    }

    if( ( ntime->second >=          60                  )
     || ( ntime->minute >=          60                  )
     || ( ntime->hour   >=          24                  )
     || ( ntime->month  >= MONTH_COUNT                  ) // Check month before day
     || ( ntime->day    >  days_per_month[ntime->month] )
     || ( ntime->year   < 2000                          )
     || ( ntime->year   > 2100                          ) )
    {
        //Data failed check. Dont take new data.
        return;
    }

    memcpy( &cur_time, ntime, sizeof(cur_time) );
}