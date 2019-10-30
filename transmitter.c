/*
 * File:   transmitter.c
 * Author: Ryan Kane
 * 
 * Code for ATmega328P transmitter
 */

#include <xc.h>
#include "header.h"

/* Function: sendChar
 * --------------------------------------------------------
 * Sends integer c in form of 8-bit binary number using transmission protocol 
 * 
 * @param ms
 *  length of delay in ms, as timer clock period is 1.02 ms
 */
void sendChar(int c) {
    int tx_bit;
    int prev = 0x00;
    
    // Send start bit
    PORTD = 0x00;
    delay(1000 / BAUD_RATE);
    
    // loops through each bit in the integer byte
    for(int bit_idx = 0; bit_idx < 8; bit_idx++) {
        tx_bit = c & (0x80 >> bit_idx); 
        // if bit is a 1, transmit high
        // else transmit low
        prev = prev << 1;
        if(tx_bit) {
            prev |= 0x01;
            PORTD = prev;
        } else {
            prev &= 0xFE;
            PORTD = prev;
        }
        delay(1000 / BAUD_RATE);
    }
    
    // Send 2 stop bits
    PORTD = 0x01;
    delay((1000 / BAUD_RATE) * 2);
}

void startTransmitter() {
    // Set GPIO port directions
    DDRD = 0xFF;
    DDRB = 0x02;

    // Transmitter code
    int transmitArray[MESSAGE_LENGTH] = {'H', 'E', 'L', 'L', 'O'};

    while(1){
        // Start Idle signal
        PORTD = 0x01;

        //delay approx 10 seconds
        for(int i = 0; i < RESET_DELAY * 4; i++) {
            delay(255);
        }

        // Put PB1 pin high, connected to LED for start signal visual
        PORTB = 0x02;

        // Send data
        for(int i = 0; i < MESSAGE_LENGTH; i++) {
            sendChar(transmitArray[i]);
        }

        // Put PB1 pin back to low
        PORTB = 0x00;
    }
}
