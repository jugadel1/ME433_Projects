#include "nu32dip.h" // constants, functions for startup and UART
#include "math.h"
#define SYSCLK 24000000.0 // system clock 
#define MAX_MESSAGE_LENGTH 200

void makeWaveform();
void delay(int t);
volatile int state = 0; 
static volatile double Waveform[100]; // waveform, used in/out of ISR so volatile

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  char message[MAX_MESSAGE_LENGTH];

  if (state == 0){
    // Starts PIC Sin Plot                
    state = 1;
    // NU32DIP_WriteUART1("Pressed!"); // debug
    for (int i = 0; i < 100; i++) {
      char message[MAX_MESSAGE_LENGTH];
      sprintf(message, "%lf\r\n", Waveform[i]);
      NU32DIP_WriteUART1(message);
      delay(100);  
    }
    state = 0;
    // delay(100);
    IFS0bits.INT0IF = 0;
  }
  else{
    // Resets PIC for next Plot
    state = 0;
    // NU32DIP_WriteUART1("Reset!"); // debug
    delay(100);     
    IFS0bits.INT0IF = 0;
  }
}
int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello!\r\n");
  makeWaveform();

  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 
  IPC0bits.INT0IS = 1;            // step 4: interrupt subpriority
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
  // Connect USER button (RA4) to INT0 (RB7)
  

  while(1){
    ; // while loop does nothing 
  }
  return 0;
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}	

void makeWaveform(){
    for (int i = 0; i < 100; ++i){
      Waveform[i] = sin(i*0.06283);
    }
}