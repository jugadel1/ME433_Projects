#ifndef SPI__H__
#define SPI__H__

#include "nu32dip.h"
#define CS LATBbits.LATB15

void initSPI();
unsigned char spi_io(unsigned char o);

#endif // SPI__H__