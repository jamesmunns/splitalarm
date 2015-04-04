#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_hibernate.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/hibernate.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"

#include "cmd.h"

int cmd_help(int argc, char **argv);

//Use the command library
tCmdLineEntry g_sCmdTable[] =
{
    {"help",     cmd_help,      " : Display list of commands" },
};

const int NUM_CMD = sizeof(g_sCmdTable)/sizeof(tCmdLineEntry);

static char input_buffer[128];

void cmd_init(void)
{
    //
    // Enable and Initialize the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    cmd_printf("\nSplitAlarm - UART Loaded\n");
    cmd_printf("> ");
}

int cmd_help(int argc, char **argv)
{
    int index;

    (void)argc;
    (void)argv;

    UARTprintf("\n");
    for (index = 0; index < NUM_CMD; index++)
    {
      UARTprintf("%17s %s\n",
        g_sCmdTable[index].pcCmd,
        g_sCmdTable[index].pcHelp);
    }
    UARTprintf("\n");

    return (0);
}

void cmd_periodic(void)
{
    long cmd_status;

    if(UARTPeek('\r') == -1)
    {
        return;
    }

    // Get the buffer
    UARTgets(input_buffer, sizeof(input_buffer));

    cmd_status = CmdLineProcess(input_buffer);

    switch(cmd_status)
    {
        case CMDLINE_BAD_CMD:
            UARTprintf("Bad command!\n");
            break;
        case CMDLINE_TOO_MANY_ARGS:
            UARTprintf("Bad arguments!\n");
            break;
        default:
            break;
    }

    UARTprintf("> ");
}