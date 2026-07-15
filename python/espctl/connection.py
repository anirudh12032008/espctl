import queue 
import threading
import serial
from serial.tools import list_ports
BAUD = 921600

ESP32 = ("CP210", "CH340", "CH9102", "FTDI", "USB-SERIAL", "USB2.0-Serial")



def findPort():
    ports = list(list_ports.comports())
    for p in ports:
        d = (p.description or "") + (p.manufacturer or "")
        if any(h.lower() in d.lower() for h in ESP32):
            return p.device
        
    if len(ports) == 1:
        return ports[0].device
    if not ports:
        raise RuntimeError("NO SERIAL PORT FOUND!!!!")
    raise RuntimeError("COULD NOT DETECT A PORT FROM: {}".format([p.device for p in ports]))




class Connection:
    def __init__(self, port=None, baud=BAUD):
        self.port = port or findPort()
        self._serial = serial.Serial(self.port, baud, timeout=1)
        self._lines = queue.Queue()
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._read_loop, daemon=True)
        self._thread.start()

    def _read_loop(self):
        while not self._stop.is_set():
            try: 
                r = self._serial.readline()
            except serial.SerialException:
                break
            if not r:
                continue
            line=r.decode("utf-8", errors="replace").rstrip("\r\n")
            if line:
                self._lines.put(line)

    def readline(self,timeout=None):
        try:
            return self._lines.get(timeout=timeout)
        except queue.Empty:
            return None
        
    def writeL(self, line):
        self._serial.write((line + "\n").encode("utf-8"))

    def close(self):
        self._stop.set()
        self._serial.close()