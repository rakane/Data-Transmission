/*
 * File:   main.c
 * Author: Ryan Kane
 * Version: 1.0
 */

#include <xc.h>

#define TRANSMIT 1  // 1 for transmitter, 0 for reciever

void delay(int ms){
    TCNT0 = 0x00;   //reset the counter to 0
    OCR0A = ms; //length of delay
    TIFR0 = 0x02;
    while(!(TIFR0 & 0x02)){}
    return;
}

void sendChar(int c) {
    
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

int main(void) {
    // Set GPIO port directions
    DDRD = 0xFF;
    DDRC = 0x00;
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
        while(!readStartTransmission()) {}
        delay(10);
        for(int i = 0; i < 5; i++) {
            receiveArray[i] = readChar();
            delay(10);
        }
        delay(255);
    }
    
    
    return 0;
}
