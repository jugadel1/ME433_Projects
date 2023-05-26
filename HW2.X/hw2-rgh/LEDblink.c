#include "nu32dip.h" // constants, functions for startup and UART
#include <limits.h>

void blink(int, int); // blink the LEDs function
void delay(int t);

int main(void) {
  char message[100];
  unsigned int iters;
  unsigned int len;
  
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  NU32DIP_WriteUART1("Hello\r\n");
  
  while (1) {
    NU32DIP_ReadUART1(message, 100); // wait here until get message from computer
    sscanf(message, "%u %u", &iters, &len);
    
    NU32DIP_WriteUART1(message); // send message back
    NU32DIP_WriteUART1("\r\n"); // carriage return and newline
	
    delay(2000); //wait 2sec to see if user button is pressed
    
    if (!NU32DIP_USER){
		  blink(iters, len); // iters times, len ms each time
      NU32DIP_WriteUART1("button executed\r\n");
	  }
    else{
      NU32DIP_WriteUART1("no button\r\n");
      blink(5, 500); // 5 times, 500ms each time

    }
  }
}

// blink the LEDs
void blink(int iterations, int time_ms){
	int i;	   
    for (i=0; i< iterations; i++){
		NU32DIP_GREEN = 0; // on
		NU32DIP_YELLOW = 1; // off     		
    delay(time_ms);
		
		NU32DIP_GREEN = 1; // off
		NU32DIP_YELLOW = 0; // on
    delay(time_ms);
	}
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}
// void delay(int time_ms){
// 	unsigned int t;
//   unsigned int a; 
//   unsigned int del;
  
//   a = UINT_MAX;
//   del = 12000*time_ms;  
  
//   t = _CP0_GET_COUNT(); // should really check for overflow here
//   if (t>a) {
//       t = a-del;
//   }
        
// 	while(_CP0_GET_COUNT() < t + del){}
// }