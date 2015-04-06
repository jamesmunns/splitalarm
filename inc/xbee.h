#ifndef _XBEE_H
#define _XBEE_H

void xbee_init(void);
void xbee_periodic_cont(void);
void xbee_periodic_second(void);
bool xbee_is_connected( void );
void xbee_periodic_second(void);

typedef enum{
    XBEE_SEEK_DELIMITER_STATE,
    XBEE_GET_SIZE_STATE,
    XBEE_GET_DATA_STATE,
    XBEE_GET_CHECKSUM_STATE
} xbee_state_machine_t;

typedef struct{
    uint8_t     frame_type;
    uint8_t     source_addr[6];
    uint8_t     rssi;
    uint8_t     options;
    uint8_t     samples;
    uint16_t    dig_mask;
    uint16_t    ana_mask;
    uint8_t     checksum;
    uint8_t     pad;
} xbee_async_dio_packet_t;

#define XBEE_DELIMITER_OFFSET    0
#define XBEE_SIZE_OFFSET         1
#define XBEE_DATA_OFFSET         3

#endif