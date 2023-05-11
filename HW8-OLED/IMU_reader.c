#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include <stdio.h>
#include "ssd1306.h"

void blink(int, int); // blink the LEDs function

int main(void) {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    NU32DIP_WriteUART1("hello!\r\n");
    init_mpu6050();
	NU32DIP_WriteUART1("IMU-Initiated!\r\n");
	
    unsigned char data_read[14]; // char array for the raw data
	float ax, ay, az, gx, gy, gz, tc; // floats to store the data
    unsigned char hoo;
    hoo = whoami(); // read whoami
    NU32DIP_WriteUART1("Whoami read");	
    
    char mess[100];
    sprintf(mess, "%X\r\n", hoo);
    NU32DIP_WriteUART1(mess); // print whoami
	
    // if whoami is not 0x68, stuck in loop with LEDs on
    if (hoo != 0x68){
        while(1){
            blink(1,200);       
        }
    }
	// wait to print until you get a newline
    unsigned char m_in[100];
    NU32DIP_ReadUART1(m_in,100);

    while (1) {
		// use core timer for exactly 100Hz loop
        _CP0_SET_COUNT(0);
        blink(1, 250);
        // blinkk(250);

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
        
        // print out the data
        char mess[100];
        sprintf(mess, "%f\r\n", az); // Print Z-axis Acceleration
        NU32DIP_WriteUART1(mess);	
        while (_CP0_GET_COUNT() < 48000000 / 2 / 100) {
        }
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
