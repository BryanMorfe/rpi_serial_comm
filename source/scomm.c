#include "scomm.h"

#include <assert.h>
#include <wiringPi.h>

#define BITS_IN_BYTES   8

#define LSB_GET_BIT( byte, i )  ( byte >> i ) & 1
#define MSB_GET_BIT( byte, i )  ( byte >> ( BITS_IN_BYTES - i - 1 ) ) & 1

#define LSB_IF_SET_BIT( byte, i, bit )  ( byte | ( bit << i ) )
#define MSB_IF_SET_BIT( byte, i, bit )  ( byte | ( bit << ( BITS_IN_BYTES - i - 1 ) )

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
    SEND,
    RECV,
    IDLE
} COMM_STATUS;

//-------------------------------------------------------------------------------------------

struct byte_context_s
{
    uint8_t* byte;
    uint8_t pos;
};

struct byte_context_s transferContext[2] = {
    [SEND] = {
        .byte = NULL,
        .pos  = 0
    },
    [RECV] = {
        .byte = NULL,
        .pos  = 0
    }
};

//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------

struct scomm_config_s
{
    uint8_t clockFrequency;
    uint8_t sendPin;
    uint8_t recvPin;
    uint8_t clockOutPin;
    uint8_t clockInPin;
    uint8_t bitOrder;
    uint8_t commMode;
    uint8_t status;
};

struct scomm_config_s config = {
    .clockFrequency = 100000, // in Hz
    .sendPin        = 17,
    .recvPin        = 18,
    .clockOutPin    = 4,
    .clockInPin     = 19,
    .bitOrder       = LSB,
    .commMode       = MASTER,
    .status         = IDLE
};

//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------

static void bitSend( void );
static void bitReceived( void );

//-------------------------------------------------------------------------------------------

void Serial_CommInit( void )
{
    wiringPiSetupGpio();
    
    pinMode( config.sendPin, OUTPUT );
    pinMode( config.recvPin, INPUT );
    pinMode( config.clockOutPin, GPIO_CLOCK );
    pinMode( config.clockInPin, INPUT );
    
}

SCOMM_ERROR_T Serial_SendByte( uint8_t byte )
{
    
    SCOMM_ERROR_T errCode = NO_ERROR;
    
    // TODO: Start Clock & Register Clock
    
    if( wiringPiISR( config.clockOutPin,  ) != 0 )
    {
        
    }
    else
    {
        
    }
    
    for( uint8_t i = 0; i < BITS_IN_BYTES; ++i )
    {
        uint8_t bit;
        if( config.bitOrder == LSB )
        {
            bit = LSB_GET_BIT( byte, i );
        }
        else
        {
            bit = MSB_GET_BIT( byte, i );
        }
        
        digitalWrite( config.dataPin, bit );
        delay( config.delayTime );
    }
    
    return errCode;
}

SCOMM_ERROR_T Serial_ReceiveByte( uint8_t* byte )
{
    
    assert( byte != NULL );
    
    SCOMM_ERROR_T retCode = NO_ERROR;
    
    if( wiringPiISR( config.clockInPin, INT_EDGE_RISING, bitReceived ) != 0 )
    {
        retCode = ISR_NO_REGISTER;
    }
    else
    {
        for( uint8_t i = 0; i < BITS_IN_BYTES; ++i )
        {
            uint8_t bit = digitalRead( config.recvPin );
            
            if( config.bitOrder == LSB )
            {
                byte = LSB_IF_SET_BIT( byte, i, bit );
            }
            else
            {
                byte = MSB_IF_SET_BIT( byte, i, bit );
            }
            
            delay( config.clockFrequency );
        }
    }
    
    
    
}

static void bitSend( void )
{
    digitalWrite(  );
}

static void bitReceived( void )
{
    uint8_t bit = digitalRead( config.recvPin );
}
