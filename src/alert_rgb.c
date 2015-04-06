#include "alert_rgb.h"
#include "drivers/rgb.h"
#include "cmd.h"

volatile unsigned long alert_rgb_colors[3];
static bool alert_rgb_alarm_flag;
static bool alert_rgb_xbee_good;
static uint32_t alert_rgb_alarm_ctr;
static uint32_t alert_rgb_xbee_ctr;

void alert_rgb_init(void)
{
    RGBInit(1);
    alert_rgb_colors[0] = 0x0000;
    alert_rgb_colors[1] = 0x0000;
    alert_rgb_colors[2] = 0x0000;

    alert_rgb_alarm_flag = false;
    alert_rgb_alarm_ctr  = 0;

    alert_rgb_xbee_good = true;
    alert_rgb_xbee_ctr = 0;

    RGBSet(alert_rgb_colors, 1.0);
    RGBEnable();
}

void alert_rgb_periodic_msec()
{
    if(alert_rgb_alarm_flag)
    {
        if( alert_rgb_alarm_ctr == 250 )
        {
            alert_rgb_colors[0] = 0xFF00;
            alert_rgb_colors[2] = 0x0000;
        }
        else if( alert_rgb_alarm_ctr == 500 )
        {
            alert_rgb_colors[0] = 0x0000;
            alert_rgb_colors[2] = 0xFF00;
            alert_rgb_alarm_ctr = 0;
        }

        alert_rgb_alarm_ctr++;

    }
    else
    {
        alert_rgb_alarm_ctr = 0;
        alert_rgb_colors[0] = 0x0000;
        alert_rgb_colors[2] = 0x0000;
    }

    if(alert_rgb_xbee_good)
    {
        alert_rgb_colors[1] = 0;
        alert_rgb_xbee_ctr = 0;
    }
    else
    {
        alert_rgb_xbee_ctr++;

        if(alert_rgb_xbee_ctr&0x1000)
        {
            alert_rgb_colors[1] = 0xFFF - (alert_rgb_xbee_ctr & 0xFFF);
        }
        else
        {
            alert_rgb_colors[1] = alert_rgb_xbee_ctr & 0xFFF;
        }

    }

    RGBColorSet(alert_rgb_colors);

}

void alert_rgb_alarm(bool alm)
{
    alert_rgb_alarm_flag = alm;
}

void alert_rgb_xbee_notify(bool xbee_good)
{
    alert_rgb_xbee_good = xbee_good;
}