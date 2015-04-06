#include "alert_rgb.h"
#include "drivers/rgb.h"
#include "cmd.h"

volatile unsigned long alert_rgb_colors[3];
static bool alert_rgb_alarm_flag;
static uint32_t alert_rgb_alarm_ctr;

void alert_rgb_init(void)
{
    RGBInit(1);
    alert_rgb_colors[0] = 0x0000;
    alert_rgb_colors[1] = 0x0000;
    alert_rgb_colors[2] = 0x0000;

    alert_rgb_alarm_flag = false;
    alert_rgb_alarm_ctr  = 0;

    RGBSet((long unsigned*)&alert_rgb_colors, 1.0);
    RGBEnable();
}

void alert_rgb_periodic_msec()
{
    if(alert_rgb_alarm_flag)
    {
        if( alert_rgb_alarm_ctr == 250 )
        {
            alert_rgb_colors[0] = 0xFFFF;
            RGBColorSet((long unsigned*)&alert_rgb_colors);
        }
        else if( alert_rgb_alarm_ctr == 500 )
        {
            alert_rgb_colors[0] = 0x0000;
            RGBColorSet((long unsigned*)&alert_rgb_colors);
            alert_rgb_alarm_ctr = 0;
        }

        alert_rgb_alarm_ctr++;

    }
}

void alert_rgb_alarm(bool alm)
{
    cmd_printf("Hello?\n");
    alert_rgb_alarm_flag = alm;
    alert_rgb_alarm_ctr = 0;
    alert_rgb_colors[0] = 0xFFFF;
    RGBColorSet((long unsigned*)&alert_rgb_colors);
}

