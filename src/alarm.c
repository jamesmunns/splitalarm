#include "clock.h"
#include "alarm.h"
#include "cmd.h"
#include <string.h>
#include "store.h"
#include "utils/ustdlib.h"

static alarm_t      alarms[ALARM_CT];
static basic_time_t lcl_time;
static bool         alarm_firing;

#define ALARM_DEBUG ( 0 )

void alarm_init(void)
{
    store_data_t eeprom;

    for(int i=0; i<ALARM_CT; i++)
    {
        alarms[i].enable_flags &= ( ~ALARM_ENABLE );
    }

    if( store_data_get(&eeprom) )
    {
        alarm_set( ALARM_CT, (alarm_t*)&eeprom.stored_alarms );
    }

    #if ALARM_DEBUG
        cmd_printf("Setting test alarm\n");
        alarms[ALARM_A].hour         = 0;
        alarms[ALARM_A].minute       = 1;
        alarms[ALARM_A].day_of_week  = ERRY_DAY;
        alarms[ALARM_A].enable_flags = ALARM_ENABLE;
    #endif

    alarm_firing = false;
}

void alarm_periodic_second(void)
{
    clock_cur_time_get(&lcl_time);

    // If it is midnight, clear all trigger flags
    if( ( lcl_time.hour   == 0 )
     && ( lcl_time.minute == 0 )
     && ( lcl_time.second == 0 ) )
    {
        for(int i=0; i<ALARM_CT; i++)
        {
            alarms[i].enable_flags &= ( ~ALARM_TRIGGERED );
        }
    }

    // Check for alarms
    bool fire_alarm = false;
    for(int i=0; i<ALARM_CT; i++)
    {
        if( alarm_check_trigger( &lcl_time, &alarms[i] ) )
        {
            fire_alarm = true;
            cmd_printf("Alarm %d fired", i);
            alarms[i].enable_flags |= ALARM_TRIGGERED;
        }
    }

    if(fire_alarm)
    {
        //BEEP BOOP
        alarm_handle_alarm();
    }
}

bool alarm_check_trigger( basic_time_t* cur_time,
                          alarm_t*      alarm )
{
    day_t cur_day;

    // Check the easy stuff
    if( ( cur_time == NULL                        )
     || ( alarm    == NULL                        )
     || ( !( alarm->enable_flags & ALARM_ENABLE ) )
     || ( alarm->enable_flags & ALARM_TRIGGERED   ) )
    {
        return false;
    }

    cur_day = clock_day_of_week_get( cur_time->year,
                                     cur_time->month,
                                     cur_time->day );

    // Do we have a match?
    if( ( cur_day & alarm->day_of_week     )
     && ( cur_time->hour   == alarm->hour   )
     && ( cur_time->minute == alarm->minute ) )
    {
        return true;
    }

    // No match
    return false;
}

void alarm_set(alarm_id_t id, alarm_t* alarm)
{

    if( id > ALARM_CT )
    {
        return;
    }
    else if( id == ALARM_CT )
    {
        for(int i = 0; i<ALARM_CT; i++)
        {
            if(!alarm_validate(&alarm[i]))
            {
                return;
            }
        }
        memcpy(alarms, alarm, sizeof(alarms));
    }
    else
    {
        if(!alarm_validate(alarm))
        {
            return;
        }
        memcpy(&alarms[id], alarm, sizeof(alarms[id]));
    }

    for(int i=0; i<ALARM_CT; i++)
    {
        alarms[i].enable_flags &= ~ALARM_TRIGGERED;
    }
}

void alarm_get(alarm_id_t id, alarm_t* alarm)
{
    if( id > ALARM_CT )
    {
        return;
    }
    else if( id == ALARM_CT )
    {
        memcpy(alarm, alarms, sizeof(alarms));
    }
    else
    {
        memcpy(alarm, &alarms[id], sizeof(alarms[id]));
    }
}

bool alarm_validate(alarm_t* alarm)
{
    if( ( alarm == NULL                 )
     || ( alarm->hour >= 24             )
     || ( alarm->minute >= 60           )
     || ( alarm->day_of_week > ERRY_DAY ) )
    {
        return false;
    }

    return true;
}

void alarm_handle_alarm(void)
{
    alarm_firing = true;
    cmd_printf("ALARM!\n");
}

void alarm_acknowledge(void)
{
    if(alarm_firing)
    {
        cmd_printf("Alarm Acknowledged!\n");
    }
    alarm_firing = false;
}

//              SMTWTFS
//ALARM_N hh:mm 1111100 [on/off]\n
void alarm_string_get( char* alarm_str )
{
    char* t_ptr = alarm_str;

    for(int i=0; i<ALARM_CT; i++)
    {
        usprintf( t_ptr,
                 //1234567   890  123  456  78 9
                  "ALARM_%01d %02d:%02d %02X %01d\n",
                  i,
                  alarms[i].hour,
                  alarms[i].minute,
                  alarms[i].day_of_week,
                  (alarms[i].enable_flags & ALARM_ENABLE) ? 1 : 0 );

        t_ptr+=19;
    }
}