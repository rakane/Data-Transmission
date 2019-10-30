/* 
 * File: header.h  
 * Author: Ryan Kane 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define BAUD_RATE 10
#define MESSAGE_LENGTH 5
#define TIMER_SCALAR 0x05
#define RESET_DELAY 5
#define TRANSMIT 1  // 1 for transmitter, 0 for reciever

extern void delay(int);
extern void startTransmitter();
extern void startReceiver();

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

