#include "nu32dip.h" // constants, functions for startup and UART
#include "ws2812b.h"

// Constants for Neopixel Strip
#define LEDS 8 // 8 per strip
#define Sat 1 // max saturation
#define Bri 1 // masx brightness

// Function Prototypes
void delay(int t);

int main(void) {
	NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
	ws2812b_setup(); // Setup TMR2 for Neopixel Control
	
	float hue_iter = 0;
	float saturation = Sat;
	float brightness = Bri;

	while(1){
		wsColor color = HSBtoRGB(hue_iter, saturation, brightness);
		ws2812b_setColor(&color, LEDS);
		delay(100);

		hue_iter++;
		if (hue_iter==360) hue_iter = 0;
	}
	
}

void delay(int t) {
  for (int i = 0; i < 12000*t; i++) {  // delays for time t in ms
      _nop(); //do nothing
  }
}