#ifndef _SCOMM_H
#define _SCOMM_H

#include <stdint.h>

typedef enum
{
    NO_ERROR,           // No error
    ISR_NO_REGISTER,    // Cannot register interrupt
    NO_RECEIVE,         // Cannot receive
    NO_SEND             // Cannot send
} SCOMM_ERROR_T;

void Serial_CommInit( void );

SCOMM_ERROR_T Serial_SendByte( uint8_t byte );

SCOMM_ERROR_T Serial_ReceiveByte( uint8_t* byte );

#endif /* _SCOMM_H */
