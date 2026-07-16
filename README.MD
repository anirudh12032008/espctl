# ESPCTL
> your ESP32 controllable from Python and your browser. No install, no cloud, 100% locally on your machine
> add one line to your firmware and get a live data + control panel

espctl is an arduino header only library for esp32 along with a python package and web serial browser client which are connected by a shared line based serial protocol ( read below for more info about the espctl protocol)

the python package could be used to colled data frames via pandas and matplotlib to plot the live graphs along with regular CRUD operations

## Why ESPCTL?
reflashing your board each time and waiting for arduino to compile everything??
trying to mentally plot a wall of `Serial.println` numbers. and nobody needs an account or cloud dashboard to drag a slider on a project that's liteterally six inches from their laptop plugged into the same usb cable

ESPCTL fixes all of that with one header file. You declare your controls in firmware, plug in, and a live plot + working widgets just show up in Python, in your browser, whatever. 
No setup, no cap.

## Features

- easy one line setup - `#include "espctl.h"`
- auto generated web ui
- easy native python access - widgets are plain attributes: `board.threshold = 120`
- data logging - record data into a csv or pull a pandas DataFrame
- live plotting - matplotlib for live plots of data
- gpio read/write - access gpio directly without tweaking the firware
- browser flashing - flash your mcu directly from the browser

## Quick Start
### 1. Arduino library
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

you can also refer to [examples/blink.ino](examples/blink.ino) for a working sketch

### 2. Python client
```sh
pip install espctl
```
```python
from espctl import ESPCtl
import time
board = ESPCtl()
print(board.bpm) #write
board.threshold = 120 #read

with board.record("session.csv"):
    time.sleep(60)
df = board.to_df() # pandas df
board.plot("bpm") # matplotlive
```

### 3. Web client
open `web/index.html` and use it directly to connect to the board




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
```

## Contributing

Issues and PRs welcome.
Good first contributions: testing on more ESP32 dev boards and reporting port detection quirks, imporving error messages, add more example sketches, add more widgets

## Scope (v0.1)
Supported: ESP32(classic) only
Widgets - slider, toggle, and value
Live plots - only 1

## future plans (v0.2)
other chips - esp8266, S3, C3
more widgets
multi plot


## License 
MIT
