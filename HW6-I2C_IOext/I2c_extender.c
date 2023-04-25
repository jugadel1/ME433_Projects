#include "nu32dip.h" // constants, functions for startup and UART
#include "math.h"
#include "i2c_master_noint.h"

#define SYSCLK 24000000.0 // system clock 
#define MAX_MESSAGE_LENGTH 200
#define s_rate 100

void makeWaveformA();
void makeWaveformB();
void delay(int t);
volatile int state = 0; 
static volatile float WaveformA[s_rate]; // waveform, used in/out of ISR so volatile
static volatile float WaveformB[s_rate];
void sendWave(float v, unsigned char a_or_b);
 
int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello!\r\n");
  initSPI();
  makeWaveformA(); // 2Hz sine wave
  makeWaveformB(); // 1Hz triangle wave
  while(1){
    for (int i = 0; i < s_rate; ++i){
      sendWave(WaveformA[i],0);
      sendWave(WaveformB[i],0);   
    }
  return 0;
  }
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}	

void makeWaveformA(){ // Wave goes from 0->1023(10-bit)
    for (int i = 0; i < s_rate; ++i){
      WaveformA[i] = 1023/2*sin(2*M_PI*i/(s_rate/2))+1023/2;
    }
}

void makeWaveformB(){ // Wave goes from 0->1023(10-bit)
    for (int i = 0; i < s_rate; ++i){
      if (i < s_rate/2){
        WaveformB[i] = i*1023/(s_rate/2);
      }
      else{
        WaveformB[i] = 1023*2-i*1023/(s_rate/2);
      }
    }
}

void sendWave(float v,unsigned char a_or_b) {
  //[a_or_b 1 1 1 [10-bit v] 0 0]
  unsigned int f = v;
  unsigned short t = 0;
  t = 0b111<<12;
  // unsigned char a_or_b; // this is 0b0 or 0b1
  t = t | (a_or_b<<15);
  t = t | (f<<2);
  
  // send voltage with SPI
  CS = 0;
  spi_io(t>>8);
  spi_io(t);//%0xFF
  CS = 1;
}