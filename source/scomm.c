#include "scomm.h"

#include <wiringPi.h>

#define BITS_IN_BYTES   8

typedef enum
{
    LSB,
    MSB
} BIT_ORDER;

typedef enum
{
    MASTER,
    SLAVE
} S_MODE;

typedef enum
{
    SENDING,
    RECEIVING,
    IDLE
} COMM_STATUS;

struct scomm_config_s
{
    uint8_t clockFrequency;
    uint8_t sendPin;
    uint8_t clockPin;
    uint8_t bitOrder;
    uint8_t commMode;
    uint8_t status;
};

struct scomm_config_s config = {
    .clockFrequency = 100000, // in Hz
    .dataPin        = 17,
    .clockPin       = 4,
    .bitOrder       = LSB,
    .commMode       = MASTER,
    .status         = IDLE
};

void Serial_CommInit( void )
{
    wiringPiSetupGpio();
    
    pinMode( config.dataPin, OUTPUT );
    pinMode( config.clockPin, GPIO_CLOCK );
}

void Serial_SendByte( byte_t byte )
{
    for( uint8_t i = 0; i < BITS_IN_BYTES; ++i )
    {
        uint8_t bit;
        if( config.bitOrder == LSB )
        {
            bit = ( byte >> i ) & 1;
        }
        else
        {
            bit = ( byte >> ( BITS_IN_BYTES - i - 1 ) ) & 1;
        }
        
        digitalWrite( config.dataPin, bit );
        delay( config.delayTime );
    }
}

uint8_t Serial_ReceiveByte( void )
{
    uint8_t byte;
    for( uint8_t i = 0; i < BITS_IN_BYTES; ++i )
    {
        uint8_t bit = digitalRead( config.dataPin );
        
        if( config.bitOrder == LSB )
        {
            byte |= ( bit << i );
        }
        else
        {
            byte |= ( bit << ( BITS_IN_BYTES - i - 1 ) );
        }
        
        delay( config.delayTime );
    }
    
    return byte;
    
}
