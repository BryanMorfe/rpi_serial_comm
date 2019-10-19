#include "scomm.h"

#include <assert.h>
#include <wiringPi.h>

#define BITS_IN_BYTES   8

#define LSB_GET_BIT( byte, i )  ( byte >> i ) & 1
#define MSB_GET_BIT( byte, i )  ( byte >> ( BITS_IN_BYTES - i - 1 ) ) & 1

#define LSB_IF_SET_BIT( byte, i, bit )  ( byte | ( bit << i ) )
#define MSB_IF_SET_BIT( byte, i, bit )  ( byte | ( bit << ( BITS_IN_BYTES - i - 1 ) )

#define IS_BITSET_SET( data, set )     ( ( data & set ) == set )

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
    uint8_t lockKeys[2];
};

struct scomm_config_s config = {
    .clockFrequency = 100000, // in Hz
    .sendPin        = 17,
    .recvPin        = 18,
    .clockOutPin    = 4,
    .clockInPin     = 27,
    .bitOrder       = LSB,
    .commMode       = MASTER,
    .status         = IDLE,
    .lockKeys       = {
        [SEND]      = 0,
        [RECV]      = 1
    },
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
    
    SCOMM_ERROR_T errCode;
    
    if( IS_BITSET_SET( config.status, COMM_SEND ) )
    {
        errCode = NO_SEND;
    }
    else if( wiringPiISR( config.clockOutPin, INT_EDGE_RISING, bitSend ) != 0 )
    {
        errCode = ISR_NO_REGISTER;
    }
    else
    {
        
        transferContext[SEND].byte = &byte;
        transferContext[SEND].pos = 0;
        
        // TODO: Start Clock
        
        piLock( config.lockKeys[SEND] ); // wait until whole byte is sent
        
        errCode = NO_ERROR;
    }
    
    return errCode;
}

SCOMM_ERROR_T Serial_ReceiveByte( uint8_t* byte )
{
    
    assert( byte != NULL );
    
    SCOMM_ERROR_T errCode = NO_ERROR;
    
    if( IS_BITSET_SET( config.status, COMM_RECV ) )
    {
        errCode = NO_RECEIVE;
    }
    else if( wiringPiISR( config.clockInPin, INT_EDGE_RISING, bitReceived ) != 0 )
    {
        errCode = ISR_NO_REGISTER;
    }
    else
    {
        transferContext[RECV].byte = byte;
        transferContext[RECV].pos = 0;
        
        piLock( config.lockKeys[RECV] ); // wait until byte is received
        
        errCode = NO_ERROR;
    }
    
    return errCode;
    
}

static void bitSend( void )
{
    uint8_t bit;
    
    if( config.bitOrder == LSB )
    {
        bit = LSB_GET_BIT( *( transferContext[SEND].byte ), pos );
    }
    else
    {
        bit = MSB_GET_BIT( *( transferContext[SEND].byte ), pos );
    }
    
    digitalWrite( config.sendPin, bit );
    
    ++pos;
    
    if( transferContext[SEND].pos == 8 )
    {
        piUnlock( config.lockKeys[SEND] );
    }
}

static void bitReceived( void )
{
    uint8_t bit = digitalRead( config.recvPin );
    
    if( config.bitOrder == LSB )
    {
        byte = LSB_IF_SET_BIT( byte, pos, bit );
    }
    else
    {
        byte = MSB_IF_SET_BIT( byte, pos, bit );
    }
    
    ++pos;
    
    if( transferContext[RECV].pos == 8 )
    {
        piUnlock( config.lockKeys[RECV] );
    }
}
