#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_hibernate.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
//#include "driverlib/hibernate.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
//#include "drivers/rgb.h"
//#include "drivers/buttons.h"
//#include "rgb_commands.h"
#include "main.h"
#include "cmd.h"
#include "clock.h"

#include "lcd44780_LP.h"

typedef unsigned char bool;

volatile bool sec_toggle;
volatile bool msec_toggle;
volatile unsigned int detick;

static char main_time_str[32];

void main_system_init();
void main_components_init();
void main_periodic();

int main(void)
{
    /* code */
    main_system_init();

    main_components_init();

    //LCDWriteText("A",0,0);

    for(;;)
    {
        main_periodic();
    }

    return 0;
}

void main_system_init()
{
    // Do all the fancy ROM startups here

    //
    // Enable stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPUStackingEnable();

    //
    // Set the system clock to run at 40Mhz off PLL with external crystal as
    // reference.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    sec_toggle = false;
    msec_toggle = false;
    detick      = 0;

    SysTickPeriodSet(SysCtlClockGet() / MAIN_SYSTICKS_PER_SEC);
    SysTickEnable();
    SysTickIntEnable();
    IntMasterEnable();
}

void main_components_init()
{
    // Call all the other _init() functions here
    cmd_init();
    clock_init();
    //led_init();
    //btn_init();
    //shift_init();
    //disp_init();
    //xbee_init();

    //temp
    //initLCD();
}

void main_periodic()
{
    //if new_millisecond
        cmd_periodic();
        //btn_periodic();
        //shift_periodic();
        //disp_periodic();
        //xbee_periodic();
    if(msec_toggle)
    {
        //Do stuff
        //cmd_periodic();

        //Reset
        msec_toggle = false;
    }

    if(sec_toggle)
    {
        //Ping
        clock_second_tick();
        //cmd_printf("TICK!\n");

        //clock_string_get( CUR_TIME_PTR,
        //                  main_time_str );
        //cmd_printf("%s\n", main_time_str );

        //Reset
        sec_toggle = false;
    }
}

void SysTickIntHandler(void)
{
    msec_toggle = true;

    if(detick==0)
    {
        sec_toggle = true;
        detick     = MAIN_SYSTICKS_PER_SEC;
    }

    detick--;

}