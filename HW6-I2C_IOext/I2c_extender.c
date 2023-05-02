#include "nu32dip.h" // constants, functions for startup and UART
#include "math.h"
#include "i2c_master_noint.h"

#define SYSCLK 24000000.0 // system clock 
#define MAX_MESSAGE_LENGTH 200

void printbitwise(unsigned char freddy);
void delay(int t);
void blink(int t);

// I2C used for chip MCP23008
void setPin(unsigned char address, unsigned char reggie, unsigned char value);
unsigned char readPin(unsigned char writeaddress, unsigned char readaddress, unsigned char reggie);

unsigned char chipaddw = 0b01000000; // A0-A2 GND, write bit at end
unsigned char chipaddr = 0b01000001; // A0-A2 GND, read bit at end

unsigned char iodirreg = 0x00; // io direction register
unsigned char iocon = 0b00000001; // GP7 output, GP0 input, all else input

unsigned char olatreg = 0x0A; // OLAT output register
unsigned char ledon = 0b00000000; // LED on (GP7 off)
unsigned char ledoff = 0b10000000; // LED off (GP7 on)

unsigned char gpioreg = 0x09; // Read GPIO register

/*
Add the I2C sample code to your project. The goal is to read from GP0, and if the button is pushed, 
turn on GP7, else turn off GP7. Blink the green LED that is connected directly to the PIC at some 
frequency as a heart-beat, so that you know the PIC is running
*/

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello!\r\n"); //startup greeting 
  i2c_master_setup(); // Initialize the I2C 1 Comm.
  
  setPin(chipaddw, iodirreg, iocon); // Initialize Extender Chip (GP0 input (button), GP7 Output (LED))
  setPin(chipaddw, olatreg, ledoff); // init GP7 LED as off
  
  int state = 0;
  unsigned char r = 1; // initialize 
  while(1){ 
    blink(100); // blink GP7 (DIAG HEATBEAT) & YELLOW
    unsigned char rn = readPin(chipaddw, chipaddr, gpioreg);
    
    if(rn){ // button not pressed
      if(state){
        setPin(chipaddw, olatreg, ledon); 
      }
      else{
        setPin(chipaddw, olatreg, ledoff);
      }

    }
    else{  // button pressed
      if(!state){
        setPin(chipaddw, olatreg, ledon); // button pressed to turn on LED
        state = 1;  // LED set to on
        delay(250); // debounce protect
      }
      else{
        setPin(chipaddw, olatreg, ledoff); // button pressed to turn off LED
        state = 0;  // LED set to off
        delay(250); // debounce protect
      }   
    }
    r = rn;
  }
}	

void blink(int t){ // = t = time in ms
  NU32DIP_YELLOW = 0;
  // setPin(chipaddw, olatreg, ledoff);
  delay(t);
  NU32DIP_YELLOW = 1;
  // setPin(chipaddw, olatreg, ledon);
  delay(t);   
}

void setPin(unsigned char address, unsigned char reggie, unsigned char value){ // register -> reggie???
  // SEND START BIT
  i2c_master_start();
  // send addy of chip
  i2c_master_send(address);
    // 0b01000000 // or 0b0100001 for read from the chip
  // send register name
    // 0x0A // OLAT SFR (i/o)
  i2c_master_send(reggie);  
  // send value to turn on chip
    // 0b10000000 to turn on, 0b00000000 to turn off (GP7)
  i2c_master_send(value);
  // send stop bit
  i2c_master_stop();
}

unsigned char readPin(unsigned char writeaddress, unsigned char readaddress, unsigned char reggie){
  // send start bit
  i2c_master_start();
  // send address with write bit
  i2c_master_send(writeaddress);
  // send register you want to read from 
  i2c_master_send(reggie);
  // restart
  i2c_master_restart();
  // send address with read bit
  i2c_master_send(readaddress);
  // receive unsigned char r
  unsigned char r = i2c_master_recv();
  // send the ACK bit (1 means no more bits)
  i2c_master_ack(1);
  // send Stop bit
  i2c_master_stop();
  return(r&0b00000001);
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}

void printbitwise(unsigned char freddy){
    char message[MAX_MESSAGE_LENGTH];
    
    for (int i = 0; i < 8; i++) {
        sprintf(message, "%d", !!((freddy << i) & 0x80));
        NU32DIP_WriteUART1(message);
    }
    NU32DIP_WriteUART1("\r\n");
}
