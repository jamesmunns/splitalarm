#include "clock.h"
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include <string.h>
#include "cmd.h"
#include "xbee.h"
#include "store.h"

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
    store_data_t eeprom;

    cur_time.year = 2015;
    cur_time.month = 1;
    cur_time.day = 1;
    cur_time.hour = 0;
    cur_time.minute = 0;
    cur_time.second = 0;

    if( store_data_get(&eeprom) )
    {
        clock_new_time_set(&eeprom.stored_time);
    }

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
        cmd_printf("%s ", timebuf);
        if(xbee_is_connected())
        {
            cmd_printf("xbee: good");
        }
        else
        {
            cmd_printf("xbee: not connected");
        }

        cmd_printf("\n");
    }
}

void clock_cur_time_get(basic_time_t* gtime)
{
    memcpy(gtime, &cur_time, sizeof(cur_time));
}

void clock_debug_toggle(bool tog)
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
    if(!clock_time_validate(ntime))
    {
        return;
    }

    memcpy( &cur_time, ntime, sizeof(cur_time) );
}

bool clock_time_validate(basic_time_t* timecheck)
{
    if(timecheck==NULL)
    {
        return false;
    }

    if( ( timecheck->second >=          60                  )
     || ( timecheck->minute >=          60                  )
     || ( timecheck->hour   >=          24                  )
     || ( timecheck->month  >= MONTH_COUNT                  ) // Check month before day
     || ( timecheck->day    >  days_per_month[timecheck->month] )
     || ( timecheck->year   < 2000                          )
     || ( timecheck->year   > 2100                          ) )
    {
        //Data failed check. Dont take new data.
        return false;
    }

    return true;
}

// Adapted from Tomohiko Sakamoto's method
day_t clock_day_of_week_get( unsigned int  y,
                             unsigned char m,
                             unsigned char d )      /* 1 <= m <= 12,  y > 1752 (in the U.K.) */
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if(m < 3)
    {
        y--;
    }
    return 1 << ( ( (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7 ) - 1 );
}