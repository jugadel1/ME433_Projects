#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include <stdio.h>
#include "ssd1306.h"
#define SYSCLK 24000000.0 // system clock 

void blink(int, int); // blink the LEDs function

int main(void) {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    NU32DIP_WriteUART1("hello!\r\n");
    
    init_mpu6050(); // Initialize IMU chip (MPU6050)
    ssd1306_setup(); // Initialize OLED screen (SSD1306)

    unsigned char data_read[14]; // char array for the raw data
	float ax, ay, az, gx, gy, gz, tc; // floats to store the data
    
    // read whoami	
    unsigned char hoo;
    hoo = whoami(); 
    // if whoami is not 0x68, stuck in loop with LEDs on
    if (hoo != 0x68){
        while(1){
            blink(1,1000);       
        }
    }

    while (1) {
		// use core timer for exactly 100Hz loop
        _CP0_SET_COUNT(0);
        NU32DIP_YELLOW = 0; // Blink Yellow LED ON
        // read IMU
        burst_read_mpu6050(data_read);
   
		// convert data
        ax = conv_xXL(data_read); // convert x-acceleration to float (g's)
        ay = conv_yXL(data_read); // convert y-acceleration to float (g's)
        az = conv_zXL(data_read); // convert z-acceleration to float (g's)
        gx = conv_xG(data_read); // convert x-gyro rate to dps
        gy = conv_yG(data_read); // convert y-gyro rate to dps
        gz = conv_zG(data_read); // convert z-gyro rate to dps
        tc = conv_temp(data_read); // convert int16_t temperature signed short to float (Celsius)
        
        // Print to Terminal
        // char mess_terminal[100];
        // sprintf(mess_terminal, "%f\r\n", az); // Print Z-axis Acceleration
        // NU32DIP_WriteUART1(mess_terminal);	

        // Draw to OLED screen
        
        char mess1[100]; // Z-Acceleration Message
        sprintf(mess1, "%f G (Z-Axis)", az); // Print Z-axis Acceleration
        drawMessage(0, 0, mess1);
        
        char mess2[100]; // Frame Build/Write time
        sprintf(mess2, "%f sec", _CP0_GET_COUNT()/SYSCLK); // Print Z-axis Acceleration
        drawMessage(0, 8*3+1, mess2);        
        
        ssd1306_update();

        // delay to 100 hz
        while (_CP0_GET_COUNT() < 48000000 / 2 / 100) {
            _nop();
        }
        NU32DIP_YELLOW = 1; // Blink Yellow LED off
    }
}

// blink the LEDs
void blink(int iterations, int time_ms) {
    int i;
    unsigned int t;
    for (i = 0; i < iterations; i++) {
        // first blink
        NU32DIP_GREEN = 0; // on
        NU32DIP_YELLOW = 1; // off
        t = _CP0_GET_COUNT(); // should really check for overflow here
                
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
        // second blink
        NU32DIP_GREEN = 1; // off
        NU32DIP_YELLOW = 0; // on
        t = _CP0_GET_COUNT(); // should really check for overflow here
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
    }
}
