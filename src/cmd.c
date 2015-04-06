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
#include "utils/cmdline.h"
#include "utils/ustdlib.h"

#include "cmd.h"
#include "clock.h"
#include "store.h"

int cmd_help(int argc, char **argv);
int cmd_time_set(int argc, char **argv);
int cmd_time_show(int argc, char **argv);
int cmd_time_debug(int argc, char **argv);
int cmd_load_eeprom(int argc, char **argv);
int cmd_save_eeprom(int argc, char **argv);
int cmd_alarm_get(int argc, char **argv);
int cmd_alarm_set(int argc, char **argv);
int cmd_alarm_clear(int argc, char **argv);
int cmd_alarm_fire(int argc, char **argv);

//Use the command library
tCmdLineEntry g_sCmdTable[] =
{
    {"help",     cmd_help,        " : Display list of commands" },
    {"timeset",  cmd_time_set,    " : Set Time 'hh mm ss dd mm yyyy'" },
    {"timenow",  cmd_time_show,   " : Display the current time"},
    {"timedbg",  cmd_time_debug,  " : on/off - Display the time every second"},
    {"getalm",   cmd_alarm_get,   " : Display all current alarms"},
    {"setalm",   cmd_alarm_set,   " : Set Alarm 'N hh mm ddddddd e', sun-sat"},
    {"eload",    cmd_load_eeprom, " : load data from the eeprom"},
    {"esave",    cmd_save_eeprom, " : save data to the eeprom"},
    {"noalm",    cmd_alarm_clear, " : stop any current alarms"},
    {"goalm",    cmd_alarm_fire,  " : fire an alarm"},
    {0,0,0}
};

const int NUM_CMD = (sizeof(g_sCmdTable)/sizeof(tCmdLineEntry)) - 1;

static char input_buffer[128];
static char output_buffer[128];

static store_data_t     eeprom_data;

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

void cmd_periodic_cont(void)
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

/**********************************************************
Command line callbacks
**********************************************************/

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

int cmd_time_set(int argc, char **argv)
{
    if(argc == 7)
    {
        basic_time_t new_time;

        //hh mm ss dd mm yyyy
        new_time.hour   = (unsigned char)ustrtoul(argv[1], 0, 10);
        new_time.minute = (unsigned char)ustrtoul(argv[2], 0, 10);
        new_time.second = (unsigned char)ustrtoul(argv[3], 0, 10);
        new_time.day    = (unsigned char)ustrtoul(argv[4], 0, 10);
        new_time.month  = (unsigned char)ustrtoul(argv[5], 0, 10);
        new_time.year   = (unsigned int )ustrtoul(argv[6], 0, 10);

        clock_new_time_set(&new_time);

        cmd_printf("Current Time:\n");
        clock_string_get( CUR_TIME_PTR,
                          output_buffer );
        cmd_printf("%s\n", output_buffer);
    }

    return 0;
}

int cmd_time_show(int argc, char **argv)
{

    cmd_printf("Current Time:\n");
    clock_string_get( CUR_TIME_PTR,
                      output_buffer );
    cmd_printf("%s\n", output_buffer);

    return 0;
}

int cmd_time_debug(int argc, char **argv)
{
    if( ( argc == 2 )
     && ( ( !ustrcmp( argv[1], "on"  ) )
       || ( !ustrcmp( argv[1], "off" ) ) ) )
    {
        unsigned char toggle;
        toggle = !ustrcmp( argv[1], "on" );

        clock_debug_toggle(toggle);
    }

    return 0;
}

int cmd_load_eeprom(int argc, char **argv)
{
    if(store_data_get(&eeprom_data))
    {
        clock_new_time_set( (basic_time_t*)&eeprom_data.stored_time);
        alarm_set( ALARM_CT, (alarm_t*)&eeprom_data.stored_alarms );
        cmd_printf("Loaded EEPROM data!\n");
    }
    else
    {
        cmd_printf("EEPROM Load Failed!\n");
    }
    return 0;
}

int cmd_save_eeprom(int argc, char **argv)
{
    alarm_get( ALARM_CT,
               (alarm_t*)&eeprom_data.stored_alarms );
    clock_cur_time_get( (basic_time_t*)&eeprom_data.stored_time);

    if(store_data_write(&eeprom_data))
    {
        cmd_printf("Stored Data\n");
    }
    else
    {
        cmd_printf("Failed to store data\n");
    }
    return 0;
}

int cmd_alarm_get(int argc, char **argv)
{
    alarm_string_get( (char*)&output_buffer );
    cmd_printf("Alarms:\n");
    cmd_printf("%s", output_buffer );

    return 0;
}

//'N hh mm ddddddd e'
int cmd_alarm_set(int argc, char **argv)
{
    if(argc == 6)
    {
        alarm_t    newalarm;
        alarm_id_t na_id;

        na_id = (alarm_id_t)ustrtoul(argv[1], 0, 10);

        if(na_id<=ALARM_CT)
        {
            day_t temp;

            newalarm.hour         = (uint8_t)ustrtoul(argv[2], 0, 10);
            newalarm.minute       = (uint8_t)ustrtoul(argv[3], 0, 10);
            temp                  = (day_t)ustrtoul(argv[4], 0, 2);
            newalarm.enable_flags = ( ((uint8_t)ustrtoul(argv[5], 0, 10)) == 1 ) ? ALARM_ENABLE : 0;

            //bit order must be reversed - too tired for something clever
            newalarm.day_of_week  = 0;
            cmd_printf("%02X:",temp);

            newalarm.day_of_week |= ( ( temp & 0x40 ) >> 6 );
            newalarm.day_of_week |= ( ( temp & 0x20 ) >> 4 );
            newalarm.day_of_week |= ( ( temp & 0x10 ) >> 2 );
            newalarm.day_of_week |= ( ( temp & 0x08 )      );
            newalarm.day_of_week |= ( ( temp & 0x04 ) << 2 );
            newalarm.day_of_week |= ( ( temp & 0x02 ) << 4 );
            newalarm.day_of_week |= ( ( temp & 0x01 ) << 6 );

            cmd_printf("%02X\n",newalarm.day_of_week);

            alarm_set(na_id, &newalarm);
        }
    }

    return 0;
}

int cmd_alarm_clear(int argc, char **argv)
{
    cmd_printf("Alarms cleared!\n");
    alarm_acknowledge();
    return 0;
}

int cmd_alarm_fire(int argc, char **argv)
{
    cmd_printf("Simulating Alarm!\n");
    alarm_handle_alarm();
    return 0;
}