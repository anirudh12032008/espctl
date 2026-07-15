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
        return w ? w->value :0;
    }


    void addToggle(String id, bool defVal){
        if ( _widgetCount >= ESPCTL_MAX_WIDGET) return;
        Widget &w = _widgets[_widgetCount++];
        w.id = id;
        w.type = WIDGET_TOGGLE;
        w.value = defVal ? 1:0;
    }
    
    bool getToggle(String id){
        Widget *w = findWidget(id);
        return w ? (w->value != 0) : false;
    }

    void addValue(String id){
        if ( _widgetCount >= ESPCTL_MAX_WIDGET) return;
        Widget &w = _widgets[_widgetCount++];
        w.id = id;
        w.type = WIDGET_VALUE;
        w.value = 0;
    }

    void addPin(int pin, String label){
        if (_pinCount >= ESPCTL_MAX_PINS) return;
        Pin &p = _pins[_pinCount++];
        p.pin = pin;
        p.label = label;
    }
    
    
    void data(String id, float value){
        Serial.print("$D");
        Serial.print(id);
        Serial.print(",");
        Serial.println(value);
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
        WIDGET_TOGGLE,
        WIDGET_VALUE
    };

    struct Widget {
        String id;
        WidgetType type;
        int min;
        int max;
        int value;
    };


    struct Pin {
        int pin;
        String label;
    };


    Widget _widgets[ESPCTL_MAX_WIDGET];
    int _widgetCount = 0;
    Pin _pins[ESPCTL_MAX_PINS];
    int _pinCount = 0;
    char _line[ESPCTL_BUF];
    int _lineLen = 0;


    Widget *findWidget(const String &id){
        for (int i = 0; i < _widgetCount; i++){
            if (_widgets[i].id == id) return &_widgets[i];
        }
        return nullptr;
    }


    void handleLine(char *line){
        if (line[0] == '\0') return;

        switch (line[0]){
            case '?': sendSchema(); break;
            // i'll add more later
            case '!': handleSet(line +1); break;
            case '>': handlePinWrite(line +1); break;
            case '<': handlePinRead(line + 1); break;
            default: break;
        }
    }

    // "!"
    void handleSet(char *rest){
        char *comma = strchr(rest, ',');
        if (!comma) return;
        *comma = '\0';
        String id = String(rest);
        int value = atoi(comma + 1);
        Widget *w = findWidget(id);
        if (w) w-> value = value;
    }



    // ">"
    void handlePinWrite(char *rest){
        char *comma = strchr(rest, ',');
        if (!comma) return;
        *comma = '\0';
        int pin = atoi(rest);
        int state = atoi(comma +1);
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state ? HIGH : LOW);

        Serial.print("$R");
        Serial.print(pin);
        Serial.print(",");
        Serial.println(state ? 1:0);
    }




    // "<"
    void handlePinRead(char *rest){
        int pin = atoi(rest);
        pinMode(pin, INPUT);
        int state = digitalRead(pin);
        Serial.print("$R");
        Serial.print(pin);
        Serial.print(",");
        Serial.println(state);
    }

    // "?"
    void sendSchema(){
        Serial.print("$S{\"widgets\":[");
        for (int i = 0; i < _widgetCount; i++){
            if (i>0) Serial.print(",");
            Widget &w = _widgets[i];
            Serial.print("{\"id\":\"");
            Serial.print(w.id);
            Serial.print("\",\"type\":\"");
            switch(w.type){
                case WIDGET_SLIDER:
                    Serial.print("slider\",\"min\":");
                    Serial.print(w.min);
                    Serial.print(",\"max\":");
                    Serial.print(w.max);
                    Serial.print(",\"value\":");
                    Serial.print(w.value);
                    break;
                case WIDGET_TOGGLE:
                    Serial.print("toggle\",\"value\":");
                    Serial.print(w.value);
                    break;
                case WIDGET_VALUE:
                    Serial.print("value\"");
                    break;
            }

            Serial.print("}");

        }
        Serial.print("], \"pins\":[");
        for (int i = 0; i < _pinCount; i++){
            if (i>0) Serial.print(",");
            Pin &p = _pins[i];
            Serial.print("{\"pin\":");
            Serial.print(p.pin);
            Serial.print(",\"label\":\"");
            Serial.print(p.label);
            Serial.print("\"}");
        }


        Serial.println("]}");

    }
};

#endif