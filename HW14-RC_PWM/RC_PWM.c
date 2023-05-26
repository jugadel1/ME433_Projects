#include "nu32dip.h" // constants, functions for startup and UART
#include "math.h"
#define SYSCLK 24000000.0 // system clock 
#define MAX_MESS 200

void delay(int t);

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello!\r\n");

  RPA0Rbits.RPA0R = 0b0101; // use pin A0 for OC1
  volatile int duty = 0; // (1600-7500 OC1RS) -> (0-180 deg)
  TRISAbits.TRISA1 = 0; // configure pin A1 to output dirxn
  
  /* 50Hz PWM, Timer3 0deg:1.5ms, 180deg:2.5ms */
  __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
  T3CONbits.TCKPS = 0b100;// Timer3 prescaler N=16 (1:16)
  PR3 = 60000 - 1;          // PR = PBCLK(48MHz)/(50KHz(Desired PWM)*TCKPS)-1
  TMR3 = 0;               // initial TMR3 count is 0
  OC1CONbits.OCTSEL = 1;  // chooses TMR3 for OC1 
  OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 7500;           // duty cycle = OC1RS/(PR3+1)
  OC1R = 7500;             // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;       // turn on Timer3
  OC1CONbits.ON = 1;      // turn on OC1
  __builtin_enable_interrupts();
  
  unsigned char buffer[100];  
  while(1){
    // NU32DIP_WriteUART1("Enter Angle (0-180 degrees):\r\n");
    // NU32DIP_ReadUART1(buffer, 100);
    // sscanf(buffer, "%d", &duty);  

    // OC1RS = (7500-1600)/180*duty + 1600; //(4500-7500)
    // sprintf(buffer, "%d Deegrees Written!\r\n", duty);
    // NU32DIP_WriteUART1(buffer);
    duty = 45;
    for (int i = 45; i < 135; i++ ){
      duty = i;
      OC1RS = (7500-1600)/180*duty + 1600;
      delay(44);
    }
  }
  return 0;
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}	