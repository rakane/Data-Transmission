/*
 * File:   receiver.c
 * Author: Ryan Kane
 *
 * Code for ATmega328P receiver
 */

#include <xc.h>
#include "header.h"

/* Function: waitStart
 * --------------------------------------------------------
 * Waits for start bit from the transmitter, then returns
 */
void waitStart() {
    int prev = 1;
    int new;
    
    new = PINC & 0x01;
    
    // waits for start signal (transition from idle to 0, or stop bit to 0)
    while (!(new == 0 && prev == 1)) {
        prev = new;
        new = PINC & 0x01;
    }
}

/* Function: readChar
 * --------------------------------------------------------
 * Receives 8-bit binary number using transmission protocol, and returns it
 * 
 * @return
 *  number received from transmitter
 */
int readChar() {
    int input_bit;
    int prev = 0x00;
    
    waitStart();
    delay((1000 / BAUD_RATE));
    
    // Drive PB1 high, indicating that a start signal has been detected
    PORTB = 0x02;
    
    // Read in data bit by bit
    for(int bit_idx = 0; bit_idx < 8; bit_idx++) {
        input_bit = PINC;
        prev = prev << 1;
        
        if(input_bit) {
            prev |= 0x01;
            PORTD = prev;
        } else {
            prev &= 0xFE;
            PORTD = prev;
        }
        
        delay(1000 / BAUD_RATE);
    }
    
    // delay during stop signal for right now, instead of detecting
    delay((1000 / BAUD_RATE));
    // Drive PB1 low, reseting start signal indicator
    PORTB = 0x00;
    PORTD = 0x00;
    delay((1000 / BAUD_RATE));
    
    return prev;
}

void startReceiver() {
    // Set GPIO port directions
    DDRD = 0xFF;
    DDRC = 0x00;
    DDRB = 0x02;

    // Receiver code
    int receiveArray[MESSAGE_LENGTH] = { 0 };

    // Display 1 on PORTD, showing receiver has started
    PORTD = 0x01;

    while(1) {
        // Read data
        for(int i = 0; i < MESSAGE_LENGTH; i++) {
            receiveArray[i]  = readChar();
        }

        // display message on PORTD
        for(int c = 0; c < MESSAGE_LENGTH; c++) {
            PORTD = receiveArray[c];
            for(int i = 0; i < 8; i++) {
                delay(((RESET_DELAY * 1020) / 40) - 10);
            }
        }
    }
}
