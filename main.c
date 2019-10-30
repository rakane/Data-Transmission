/*
 * File:   main.c
 * Author: Ryan Kane
 * Version: 1.0
 * 
 * Driver program for transmitter and receiver
 * 
 */

#include <xc.h>
#include "header.h"

/* Function: delay
 * --------------------------------------------------------
 * Creates delay of length ms
 * 
 * @param ms
 *  length of delay in ms, as timer clock period is 1.02 ms
 */
void delay(int ms){
    TCNT0 = 0x00;   //reset the counter to 0
    OCR0A = ms; //length of delay
    TIFR0 = 0x02;
    while(!(TIFR0 & 0x02)){}
}

int main(void) {

    // timer pre-scalar set to 1024
    // Creates a 976.5 Hz clock, or 1.02 ms period
    TCCR0B = TIMER_SCALAR;
    
    if(TRANSMIT) {
        startTransmitter();
    } else {
        startReceiver();
    }
    
    return 0;
}
