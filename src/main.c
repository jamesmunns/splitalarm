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
#include "alarm.h"
#include "xbee.h"
#include "store.h"

#include "lcd44780_LP.h"

volatile bool sec_toggle;
volatile bool msec_toggle;
volatile unsigned int detick;
volatile uint32_t cur_ms_tick;

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

    sec_toggle  = false;
    msec_toggle = false;
    detick      = 0;
    cur_ms_tick = 0;

    // Enable the real time clock peripheral
    SysTickPeriodSet(SysCtlClockGet() / MAIN_SYSTICKS_PER_SEC);
    SysTickEnable();
    SysTickIntEnable();
    IntMasterEnable();
}

void main_components_init()
{
    // Call all the other _init() functions here

    // Initialize cmd first to get printing
    // Uses PortA pins 0,1
    cmd_init();

    // All other inits. May be order dependant

    // Make EEProm available
    store_init();

    // Initialize the clock tracking
    clock_init();

    // Initialize the alarm tracking
    alarm_init();

    // Initialize the XBee radio
    // Uses PortC pins 4,5
    xbee_init();
    //led_init();
    //btn_init();
    //shift_init();
    //disp_init();

    //temp
    // Uses PortB pins 0,1,4,5,6,7
    //initLCD();
}

void main_periodic()
{
    // Every cycle
    cmd_periodic_cont();
    xbee_periodic_cont();

    // TODO
    //btn_periodic();
    //shift_periodic();
    //disp_periodic();

    if(msec_toggle)
    {
        //Do stuff
        cur_ms_tick++;

        //Reset
        msec_toggle = false;
    }

    if(sec_toggle)
    {
        //Ping
        clock_periodic_second();
        alarm_periodic_second();


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

uint32_t main_ms_tick_get(void)
{
    return cur_ms_tick;
}