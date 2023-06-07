#ifndef ICONTROL__H__
#define ICONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32DIP.h"

#include "utilities.h"
#include "NU32DIP.h" // constants, functions for startup and UART
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "pcontrol.h"
void icontrolstartup();

#endif // UTILITIES__H__
