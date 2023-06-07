#include "icontrol.h"

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) icontroller(void){  // _TIMER_2_VECTOR = 8
    int d;
    int dirxn;

    static float adcval = 0;
    static float ma;
    ma = get_pcontrol_iref();
    adcval = INA219_read_current();
    
    // calculate u using control values
    static int e = 0;                       
    e =  ma - adcval; // calc e
    int eint = get_ieint() + e;
    if (eint > 1000){ // integrator anti-windup
        eint = 1000;
    }
    if (eint < -1000){
        eint = -1000;
    }
    set_ieint(eint);

    float iKi = get_igain_ki();
    float iKp = get_igain_kp();

    float u = iKp*e + iKi*eint; // calc u    
    float unew = u + 0; // treat u as % centered at 0%
    if (unew > 100.0){
        unew = 100.0;
        dirxn = 1;
    }
    if (unew > 0 && unew <100){
        unew = unew;
        dirxn = 1;
    }
    if (unew < 0 && unew > -100.0){
        unew = -unew;
        dirxn = 0;  //unsure
    }
    if (unew < -100.0){
        unew = 100.0;
        dirxn = 0;  //unsure
    }
    OC1RS = (unsigned int) ((unew/100.0) * PR3); // set duty based on PWM
    LATAbits.LATA1 = dirxn; // set direction pin
    IFS0bits.T2IF = 0; // clear interrupt flag
    
}

void icontrolstartup(){

    RPA0Rbits.RPA0R = 0b0101; // use pin A0 for OC1 *** NEEDS CHANGE ***
    RPA1Rbits.RPA1R = 0b0101; // use pin A1 for OC2 *** NEEDS CHANGE ***
    
    volatile int duty = 25;
    volatile int dir = 0;
    TRISAbits.TRISA1 = 0; // configure pin A1 to output dirxn
    
    /* Motor 1: 20 kHz PWM, Timer3, duty cycle 0% */
    __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
    T3CONbits.TCKPS = 0;    // Timer3 prescaler N=1 (1:1)
    PR3 = 2400 - 1;         // PR = PBCLK(48MHz)/20KHz(Desired PWM) - 1
    TMR3 = 0;               // initial TMR3 count is 0
    OC1CONbits.OCTSEL = 1; // chooses TMR3 for OC1 
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0;              // duty cycle = OC1RS/(PR3+1) = 75%
    OC1R = 0;               // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;       // turn on Timer3
    OC1CONbits.ON = 1;      // turn on OC1

    /* Motor 2:  20 kHz PWM, Timer3, duty cycle 0% */
    OC2CONbits.OCTSEL = 1; // chooses TMR3 for OC2 
    OC2CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC2RS = 0;              // duty cycle = OC2RS/(PR3+1) = 75%
    OC2R = 0;               // initialize before turning OC2 on; afterward it is read-only
    OC2CONbits.ON = 1;      // turn on OC2

    // 5 Khz TMR2 ISR *** NEEDS CHANGE (Maybe TMR4? Diff Freq)***
    PR2 = 9600 - 1;                // set period register
    TMR2 = 0;                       // initialize count to 0
    T2CONbits.TCKPS = 0;            // set prescaler to 1
    T2CONbits.ON = 1;               // turn on Timer2
    IPC2bits.T2IP = 5;              // INT step 4: priority
    IPC2bits.T2IS = 0;              // subpriority
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;              // INT step 6: enable interrupt
    __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU
    // end of initialize interrupt for waveform
}

