/*
 * File:   main.c
 * Author: Ryan Kane and Isaiah Plummer
 * Version: 1.0
 */

#include <xc.h>

#define TRANSMIT 1  // 1 for transmitter, 0 for reciever

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
    return;
}

/* Function: sendChar
 * --------------------------------------------------------
 * Sends integer c in form of 8-bit binary number using transmission protocol 
 * 
 * @param ms
 *  length of delay in ms, as timer clock period is 1.02 ms
 */
void sendChar(int c) {
    
    int tx_bit;
    
    // loops through each bit in the integer byte
    for(int bit_id = 0; bit_id < 8; bit_id++) {
        tx_bit = c & (0x80 >> bit_id); 
        
        // if bit is a 1, transmit high
        // else transmit low 
        if(tx_bit) {
            PORTD = 0x01;
        } else {
            PORTD = 0x00;
        }
        delay(5);
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
    int input;
    int data = 0;
    int scalar = 128;
    
    for(int i = 0; i < 8; i++) {
        input = PINC;
        data += scalar * input;
        scalar = scalar / 2;
        delay(5);
    }
    
    return data;
}

/* Function: sendStartTransmission
 * --------------------------------------------------------
 * Sends integer 2 in form of 8-bit binary number using transmission protocol
 * Signifies the start of transmission to reciever
 */
void sendStartTransmission() {
    int c = 2;
    
    int tx_bit;
    
    for(int bit_id = 0; bit_id < 8; bit_id++) {
        tx_bit = c & (0x80 >> bit_id); 
        if(tx_bit) {
            PORTD = 0x01;
        } else {
            PORTD = 0x00;
        }
        delay(5);
    }
}

/* Function: readStartTransmission
 * --------------------------------------------------------
 * Reads input from transmitter and compares it with 2, the number signifying
 * the start of transmission
 */
int readStartTransmission() {
    int input;
    int data = 0;
    int scalar = 128;
    
    for(int i = 0; i < 8; i++) {
        input = PINC;
        data += scalar * input;
        scalar = scalar / 2;
        delay(5);
    }
    
    if(data == 2) {
        return 1;
    }
    return 0;
}

int main(void) {
    // Set GPIO port directions
    DDRB = 0xFF;
    DDRC = 0x00;
    DDRD = 0xFF;
    // timer pre-scalar set to 1024, creating clock period of 1.02 ms
    TCCR0B = 0x05;
    
    if(TRANSMIT) {
        // Transmitter code
        int transmitArray[] = {'H', 'E', 'L', 'L', 'O'};
    
        while(1){
            sendStartTransmission();
            delay(10);
            for(int i = 0; i < 5; i++) {
                sendChar(transmitArray[i]);
                delay(10);
            }
            PORTD = 0x00;
            delay(255);
        }
    } else {
        // Receiver code
        int receiveArray[] = {0, 0, 0, 0, 0};
        
        while(1) {
            while(!readStartTransmission()) {}
            delay(10);
            for(int i = 0; i < 5; i++) {
                receiveArray[i] = readChar();
                delay(10);
            }
            delay(255);
        }
    }
    
    return 0;
}
