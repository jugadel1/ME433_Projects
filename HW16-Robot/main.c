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

    int PRx = 2400-1;
    while(1){
        // int d1;
        // int d2;
        // int P1;
        // int P2;
        char buffer[MAX_MESS];
        // NU32DIP_ReadUART1(buffer, 100);
        // sscanf(buffer, "%d %d %d %d", &d1, &d2, &P1, &P2); 
        
        // unsigned int PWM1 = (unsigned int) ((P1/100.0) * PRx);
        // unsigned int PWM2 = (unsigned int) ((P2/100.0) * PRx);
        // sprintf(buffer, "dir1: %d, dirmak2: %d, PWM1: %d, PWM2: %d!\r\n", d1,d2,PWM1,PWM2);
        // NU32DIP_WriteUART1(buffer);
        
        // LATBbits.LATB14 = d1; // set direction pin OC1RS
        // LATBbits.LATB12 = d2; // set direction pin OC2RS
        // OC1RS = PWM1; // set duty based on PWM
        // OC2RS = PWM2; // set duty based on PWM
    int com = 0;
    // uart2_flag() is 1 when uart2 has rx a message and sprintf'd it into a value
    if(get_uart2_flag()){
            set_uart2_flag(0); // set the flag to 0 to be ready for the next message
            com = get_uart2_value();
            sprintf(buffer,"%d\r\n",com);
            NU32DIP_WriteUART1(buffer);
    }     
    
    com = com - 30;
    float KpL = 0.1;
    float KpR = 0.1;
    
    float uL = KpL*com;
    float unewL = uL + 0;
    if (unewL > 100.0){
        unewL = 100.0;
    } 
    unsigned int PWM1 = (unsigned int) ((unewL/100.0) * PRx);
    float uR = KpR*com;
    float unewR = uR + 0;
    if (unewR > 100.0){
        unewR = 100.0;
    }
    unsigned int PWM2 = (unsigned int) ((unewR/100.0) * PRx);
    }
}