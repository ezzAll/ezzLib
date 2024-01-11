#ifndef KB_H
#define KB_H

//#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>


uint8_t PinRead(uint8_t port); 
uint8_t AnalogRead(uint8_t port);

void digitalPinWrite(uint8_t port,uint8_t e);
void SetDigitalPinMode(uint8_t port, bool mode);
void SetAnalogPinMode(uint8_t port, uint8_t mode);

void digitalInterrupt(uint8_t port,void (*function)(),uint8_t mode);
void SerialPort();
void send_byte(uint8_t byte);
void send_buffer(char * buffer);
#endif
