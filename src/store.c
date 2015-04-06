#include "store.h"
#include "utils/crc.h"
#include "driverlib/eeprom.h"
#include "cmd.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include <string.h>

static bool             store_eeprom_available;
static bool             store_data_available;
static store_data_t     store_shadow_data;

void store_init(void)
{
    uint32_t ret = EEPROM_INIT_RETRY;

    store_eeprom_available = false;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    while(ret!=EEPROM_INIT_OK)
    {
        ret = EEPROMInit();
        if( ret == EEPROM_INIT_ERROR )
        {
            cmd_printf("eeprom init fail\n");
            return;
        }
    }

    store_eeprom_available = true;

    store_data_available = store_shadow_load();
    if(store_data_available)
    {
        cmd_printf("eeprom load worked\n");
    }

}

bool store_shadow_load()
{
    if(!store_eeprom_available)
    {
        return false;
    }

    EEPROMRead( (unsigned long int*)&store_shadow_data,
                0x0,
                sizeof(store_shadow_data) );

    if(STORE_CURRENT_DATA_VERSION != store_shadow_data.stored_version )
    {
        return false;
    }

    uint32_t t_crc = Crc32( STORE_CRC_SEED,
                            (unsigned char*)&store_shadow_data,
                            sizeof(store_shadow_data) - sizeof(store_shadow_data.stored_crc) );

    return (t_crc == store_shadow_data.stored_crc);
}

bool store_data_get(store_data_t* data)
{
    if( ( !store_eeprom_available )
     || ( !store_data_available   ) )
    {
        return false;
    }

    memcpy(data, &store_shadow_data, sizeof(store_shadow_data) );
    return true;
}

bool store_data_write( store_data_t* data )
{
    if( ( !store_eeprom_available )
     || ( NULL == data            ) )
    {
        return false;
    }

    // Fill out the version of data to store
    data->stored_version = STORE_CURRENT_DATA_VERSION;

    // Fill out the CRC of data to store
    data->stored_crc = Crc32( STORE_CRC_SEED,
                              (unsigned char*)data,
                              sizeof(store_data_t) - sizeof(store_shadow_data.stored_crc) );

    uint32_t ret = EEPROMProgram( (unsigned long int*)data,
                                  0x0,
                                  sizeof(store_data_t) );

    if( ret != 0 )
    {
        store_eeprom_available = false;
        store_data_available   = false;
        return false;
    }

    // Update the shadow register if the write was good
    memcpy(&store_shadow_data, data, sizeof(store_shadow_data) );
    store_data_available = true;

    return true;


}