# ESPCTL
your ESP32 controllable from Python and your browser. No install, no cloud, 100% locally on your machine

espctl is an arduino header only library for esp32 along with a python package and web serial browser client which are connected by a shared line based serial protocol ( read below for more info about the espctl protocol)


## Quick Start

### Arduino library
copy `firmware/espctl.h` to your sketch and:
```cpp
#include "espctl.h"
ESPCtl board;

void setup(){
    board.begin() // serial @ 921600 baud
    board.addSlider("threshold", 0, 255, 100)
    board.addToggle("ledOn", true);
    board.addValue("bpm");
}
void loop(){
    board.data("bpm", readBPM());
    int t = board.getSlider("threshold");
    board.update // MUST BE CALLED EVERY LOOP
}
```


## espctl protocol:
all msg are new lines over serial - baud 921600

### firmware to client
```
Schema sent on boot
$S{"widgets":[{"id":"bpm","type":"value"},
{"id":"threshold", "type":"slider","min":0,"max":255,"value":100}]}
```

```
data update
$D<id>,<value>

pin state
$R<pin>,<state>

anything without "$"
normal log
```

### client to firmware
```
? -> request schema
!<id>,<value> -> set a widget
><pin>,<0|1> -> write GPIO
<<pin> -> read GPIO