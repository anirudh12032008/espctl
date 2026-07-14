#ifndef ESPCTL_H
#define ESPCTL_H

#include <Arduino.h>

#define ESPCTL_MAx_WIDGET 16
#define ESPCTL_MAX_PINS 16

#define ESPCTL_BUF 128





// gng I am doing a C library for the first time please don't judge me </3

class ESPCtl {


    public:
    void begin(int baud = 921600){
        Serial.begin(baud);
        _lineLen = 0;
    }
    void update(){
        while (Serial.available()){
            char c = Serial.read();
            if (c == '\n'){
                _line[_lineLen] = '\0';
                handleLine(_line);
                _lineLen = 0;
        } else if ( c != '\r' && _lineLen < ESPCTL_BUF - 1){
                _line[_lineLen++] = c;
            }
        }
    }
}