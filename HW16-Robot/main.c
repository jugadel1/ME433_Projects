#include "nu32dip.h" // constants, functions for startup and UART
#include "uart2.h"
#include "icontrol.h"
#define SYSCLK 24000000.0 // system clock 
#define MAX_MESS 200

int main(void) {
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    NU32DIP_WriteUART1("Hello!\r\n");
    NU32DIP_GREEN = 0;  // turn on ON LED
    NU32DIP_YELLOW = 1;   // turn off problem LED
    
    __builtin_disable_interrupts();
    UART2_Startup();
    icontrolstartup();
    __builtin_enable_interrupts();



}