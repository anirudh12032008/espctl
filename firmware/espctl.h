#ifndef ESPCTL_H
#define ESPCTL_H

#include <Arduino.h>

#define ESPCTL_MAX_WIDGET 16
#define ESPCTL_MAX_PINS 16

#define ESPCTL_BUF 128





// gng I am doing a C library for the first time please don't judge me </3

class ESPCtl {


    public:
    void begin(int baud = 921600){
        Serial.begin(baud);
        _lineLen = 0;
    }



    void addSlider(String id, int min, int max, int defVal){
        if ( _widgetCount >= ESPCTL_MAX_WIDGET) return;
        Widget &w = _widgets[_widgetCount++];
        w.id = id;
        w.type = WIDGET_SLIDER;
        w.min = min;
        w.max = max;
        w.value = defVal;
    }
    int getSlider(String id){
        Widget *w = findWidget(id);
        return w ? (w->value != 0) : false;
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











    private:
    enum WidgetType {
        WIDGET_SLIDER,
    };

    struct Widget {
        String id;
        WidgetType type;
        int min;
        int max;
        int value;
    }


    Widget _widgets[ESPCTL_MAX_WIDGET];
    int _widgetCount = 0;
    Pin _pins[ESPCTL_MAX_PINS];
    int _pinCount = 0;
    char _line[ESPCTL_BUF];
    int _lineLen = 0;


    Widget *findWidget(const String &id){
        for (int i = 0; i < _widgetCount; i++){
            if (_widget[i].id == id) return &_widgets[i];
        }
        return nullptr;
    }


    void handleLine(char *line){
        if (line[0] == '\0') return;

        switch (line[0]){
            case '?': sendSchema(); break;
            // i'll add more later
        }
    }
}