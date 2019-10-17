#ifndef _SCOMM_H
#define _SCOMM_H

#include <stdint.h>

void Serial_CommInit( void );

void Serial_SendByte( uint8_t byte );

uint8_t Serial_ReceiveByte( void );

#endif /* _SCOMM_H */
