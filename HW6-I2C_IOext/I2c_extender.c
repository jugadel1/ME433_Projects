#include "nu32dip.h" // constants, functions for startup and UART
#include "math.h"
#include "i2c_master_noint.h"

#define SYSCLK 24000000.0 // system clock 
#define MAX_MESSAGE_LENGTH 200

void delay(int t);
void blink(int t);
void setPin(unsigned char address, unsigned char register, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char register);

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello!\r\n"); //startup greeting 
  i2c_master_setup(); // Initialize the I2C 1 Comm.
  // Initialize Extender Chip (GP0 input (button), GP7 Output (LED))
  
  while(1){
    blink(500); // Diag heartbeat
    // blink GP7 (DIAG HEATBEAT)

    // r = read_from_GP0
    // if(r)
    //   turn on GP7 
    // else
    //   turn off GP7

  }
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}	
void setPin(unsigned char address, unsigned char register, unsigned char value){
  // SEND START BIT
  // send addy of chip
      // 0b01000000 // or 0b0100001 for read from the chip
  // send register name
      // 0x0A // OLAT SFR (i/o)
  // send value to turn on chip
      // 0b10000000 to turn on, 0b00000000 to turn off (GP7)
  // send stop bit
}

unsigned char readPin(unsigned char address, unsigned char register){
  // send start bit
  // send address with write bit
  // send register you want to read from 
  // restart
  // send address with read bit
  //
}

void blink(int t){ // = t = time in ms
  while(1){
    NU32DIP_YELLOW = 0;
    delay(t);
    NU32DIP_YELLOW = 1;
    delay(t);    
  }
}