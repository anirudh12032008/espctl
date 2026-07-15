#include "espctl.h"

ESPCtl board;
const int LED_PIN = 2;

void setup(){
    board.begin();
    board.addSlider("blinkDelay", 50, 2000, 500);
    board.addToggle("ledOn", true);
    board.addValue("uptime");
    pinMode(LED_PIN, OUTPUT);
}

void loop(){
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    int = delayMs = board.getSlider("blinkDelay");
    bool enabled = board.getToggle("ledOn");


    if ( enabled && millis() - lastBlink >= (unsigned long) delayMs){
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        lastBlink = millis();
    } else if (!enabled) {
        digitalWrite(LED_PIN, LOW);
    }

    board.data("uptime", millis() / 1000.0);
    board.update();
}