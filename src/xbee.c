#include "misc.h"
#include "driverlib/rom.h"
#include "cmd.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "xbee.h"
#include "main.h"
#include "alarm.h"
#include "alert_rgb.h"

#define XBEE_UART_PERIPH SYSCTL_PERIPH_UART1
#define XBEE_UART_BASE   UART1_BASE
#define XBEE_PORT        SYSCTL_PERIPH_GPIOC
#define XBEE_DIG_RD_ID
#define XBEE_DIO7_BYTE   17
#define XBEE_DIO7_MASK   0x01

// We expect a message once per minute. Allow one packet miss
#define XBEE_WORRY_TIME_MS  ( (uint32_t)(1000 * 60 * 2.5) ) // * 60

static xbee_state_machine_t current_state;
static uint8_t              xbee_buffer[128];
static uint8_t              xbee_buf_index;
static uint16_t             size;
static uint32_t             last_byte_ms;
static uint32_t             last_ping_ms;

// Prototypes
void xbee_depacket_machine(uint8_t cur_byte);

// Functions
void xbee_init(void)
{
    // Configure the peripheral
    ROM_SysCtlPeripheralEnable(XBEE_PORT);
    ROM_GPIOPinConfigure(GPIO_PC4_U1RX);
    ROM_GPIOPinConfigure(GPIO_PC5_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);


    current_state  = XBEE_SEEK_DELIMITER_STATE;
    xbee_buf_index = XBEE_DELIMITER_OFFSET;

    last_byte_ms = main_ms_tick_get();
    last_ping_ms = last_byte_ms;

    //Set up the UART
    ROM_SysCtlPeripheralEnable( XBEE_UART_PERIPH ); //ulUARTPeriph
    ROM_UARTConfigSetExpClk( XBEE_UART_BASE, //ulbase
                             ROM_SysCtlClockGet(),
                             9600,
                             ( UART_CONFIG_PAR_NONE
                             | UART_CONFIG_STOP_ONE
                             | UART_CONFIG_WLEN_8 ) );

    // We are NOT using interrupt drivers here.
    // TODO use interrupts, maybe util/ringbuf.c?

    // Enable the UART
    ROM_UARTEnable( XBEE_UART_BASE );
}

void xbee_periodic_cont(void)
{
    uint32_t utemp;
//    bool has_announced = false;
    while(ROM_UARTCharsAvail(XBEE_UART_BASE))
    {
        //We can block because there is a char available
        utemp = ROM_UARTCharGet(XBEE_UART_BASE);
        if(utemp&0xFFFFFF00)
        {
            cmd_printf("XBEE_ERROR_0x%08X ", utemp);
        }
        else
        {
            xbee_depacket_machine( ( uint8_t )( utemp & 0xFF ) );
        }
    }

}

void xbee_periodic_second(void)
{
    alert_rgb_xbee_notify(xbee_is_connected());
}

void xbee_depacket_machine(uint8_t cur_byte)
{
    uint8_t  cksm = 0;
    uint32_t t_tick;

    t_tick = main_ms_tick_get();

    if( ( xbee_buf_index >= 128           )   // Too much data in buffer
     || ( ( t_tick - last_byte_ms ) >= 10 ) ) // Quiet for 10ms: timeout
    {
        current_state  = XBEE_SEEK_DELIMITER_STATE;
        xbee_buf_index = XBEE_DELIMITER_OFFSET;
    }

    last_byte_ms = t_tick;
    xbee_buffer[xbee_buf_index] = cur_byte;
    xbee_buf_index++;

    switch(current_state)
    {
        case XBEE_SEEK_DELIMITER_STATE:

            // Dont do anything until we find the delimiter
            if( cur_byte == 0x7E )
            {
                current_state  = XBEE_GET_SIZE_STATE;
            }
            break;

        case XBEE_GET_SIZE_STATE:

            // We've gotten both bytes
            if(xbee_buf_index >= XBEE_DATA_OFFSET)
            {
                current_state = XBEE_GET_DATA_STATE;

                // Parse the size
                size  = ( (uint16_t)xbee_buffer[XBEE_SIZE_OFFSET] << 8 ) & 0xFF00;
                size |= xbee_buffer[XBEE_SIZE_OFFSET+1];
            }
            break;

        case XBEE_GET_DATA_STATE:

            if( ( xbee_buf_index-XBEE_DATA_OFFSET ) >= ( size ) )
            {
                current_state = XBEE_GET_CHECKSUM_STATE;
            }

            break;

        case XBEE_GET_CHECKSUM_STATE:

            //Sum everything, including the checksum byte
            for(int i = XBEE_DATA_OFFSET; i<xbee_buf_index; i++)
            {
                cksm += xbee_buffer[i];
            }

            if(cksm==0xFF)
            {
                // Good message. First debug
                cmd_printf("Good XBEE Message! - ");
                for(uint8_t f=0; f<xbee_buf_index; f++)
                {
                    cmd_printf("%02X ", xbee_buffer[f]);
                }
                cmd_printf("\n");

                // Record this as the last ping time
                last_ping_ms = t_tick;

                // Correct packet ID type
                if( ( xbee_buffer[3]  == 0x82 )   // RX 64bit IO info
                 && ( xbee_buffer[17] == 0x01 ) ) // We are "Active High"
                {
                    alarm_acknowledge();
                }

            }
            else
            {
                cmd_printf("Bad XBEE Message!\n");
            }

            // Intentional fallthrough to default
        default:
            current_state  = XBEE_SEEK_DELIMITER_STATE;
            xbee_buf_index = XBEE_DELIMITER_OFFSET;
            break;
    }
}

bool xbee_is_connected( void )
{
    // Give the remote time to come online after powerup or rollover
    if( main_ms_tick_get() < XBEE_WORRY_TIME_MS )
    {
        return true;
    }

    return ( ( last_ping_ms + XBEE_WORRY_TIME_MS ) > main_ms_tick_get() );
}