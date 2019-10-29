/*
 * File:   main.c
 * Author: Ryan Kane
 * Version: 1.0
 */

#include <xc.h>

#define BAUD_RATE 10
#define MESSAGE_LENGTH 5
#define TIMER_SCALAR 0x05
#define RESET_DELAY 5
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
}

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


/* Function: readChar
 * --------------------------------------------------------
 * Receives 8-bit binary number using transmission protocol, and returns it
 * 
 * @return
 *  number received from transmitter
 */
int readChar() {
    int input_bit;
//    int data_byte = 0;
//    int scalar = 128;
    int prev = 0x00;
    
    waitStart();
    delay((1000 / BAUD_RATE));
    
    // Drive PB1 high, indicating that a start signal has been detected
    PORTB = 0x02;
    
    // Read in data bit by bit
    for(int bit_idx = 0; bit_idx < 8; bit_idx++) {
        input_bit = PINC;
//        data_byte += scalar * input_bit;
//        scalar = scalar / 2;
        
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


int main(void) {

    // timer pre-scalar set to 1024
    // Creates a 976.5 Hz clock, or 1.02 ms period
    TCCR0B = TIMER_SCALAR;
    
    if(TRANSMIT) {
        
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
    } else {
        
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
                receiveArray[i] = readChar();
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
    
    return 0;
}
