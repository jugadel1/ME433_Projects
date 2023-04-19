# ME433

## HW2 - Alternate LED blinks using PIC32MX170F256B

- initializes A4 as input, and B4 and B5 as output
- in the infinite loop, wait until a newline is received from the computer, print it back, and if the USER button is not pressed, blink the LEDs 5 times over 2.5 seconds
- Edit the code to receive two numbers from the user over the serial port, the number of times to blink and how long each blink period should be.

## HW3 - Plot to terminal using UART using PIC32MX170F256B

- Every time you push the USER button, sends a single cycle of a sine wave in 100 data points to UART, with a 0.01 second delay between each data point.

- Uses sin_plotter.py to view the data as a graph.
