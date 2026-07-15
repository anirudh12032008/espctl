import csv 
import time
from contextlib import contextmanager
from . import protocol
from .connection import Connection



_INT_ATTRS={
    "_conn",
    "_widgets",
    "_values",
    "_schema_event",
    "_recording",
    "_record_rows",
    "_plot_buffers"
}


class ESPCtl:
    def __init__(self, port=None, timeout=5):
        self._conn = Connection(port=port)
        self._widgets = {}
        self._values = {}
        self._recording = False
        self._record_rows = None
        self._plot_buffers = {}
        self._request_schema(timeout=timeout)

    def _request_schema(self, timeout=5):
        self._conn.write_line(protocol.REQUEST_SCHEMA)
        d = time.time() + timeout
        while time.time()< d:
            l = self._conn.readline(timeout=d - time.time())
            if l is None:
                break
            self._handle_line(l)
            if self.widgets:
                return
        raise TimeoutError("NO SCHEMA")
    def _handle_line(self, line):
        s = protocol.parseSch(line)
        if s is not None:
            self._widgets = {w["id"]: w for w in s.get("widgets", [])}
            for wid, w in self._widgets.items():
                if "value" in w:
                    self._values[wid] = w["value"]
            return
        data = protocol.parseData(line)
        if data is not None:
            wid, value = data
            self._values[wid] = value
            if wid in self._plot_buffers:
                self._plot_buffers[wid].append((time.time(), value))
            if self._recording:
                self._record_rows.append((time.time(), wid, value))
            return
        


    def poll(self, timeout=0):
        while True:
            l = self._conn.readline(timeout=timeout)
            if l is None:
                return
            self._handle_line(l)

    def __getattr__(self, name):
        if name in _INT_ATTRS:
            raise AttributeError(name)
        self.poll()
        if name in self._values:
            return self._values[name]
        if name in self._widgets:
            return None
        raise AttributeError("NO WIDGET NAMED {!r}".format(name))
    
    def __setattr__(self, name, value):
        if name in _INT_ATTRS or name.startswith("__"):
            object.__setattr__(self, name, value)
            return
        widgets = self.__dict__.get("_widgets", {})
        if name in widgets:
            self._conn.writeL(protocol.formatSet(name, value))
            self._values[name] = value
        else:
            object.__setattr__(self, name, value)


    @contextmanager
    def record(self, path):
        self._recording = True
        self._record_rows = []
        try:
            yield self
        finally:
            self._recording = False
            with open(path, 'w', newline="") as f:
                writer = csv.writer(f)
                writer.writerow(["timestamp", "widget_id", "value"])
                writer.writerows(self._record_rows)


    def to_df(self):
        import pandas as pd # finally back to the peak libs
        rows = self._record_rows or []
        return pd.DataFrame(rows, columns=["timestamp", "widget_id", "value"])
    def plot_live(self, id, s=30);
        import matplotlib.pyplot as plt
        from matplotlib.animation import FuncAnimation

        buf = self._plot_buffers.setdefault(id, [])
        fig, ax = plt.subplots()
        line, = ax.plot([], [])
        ax.set_xlabel("time (s)")
        ax.set_ylabel(id)

        def update(_frame):
            self.poll()
            if not buf:
                return line,
            now = time.time()
            cutoff = now - s
            while buf and buf[0][0] < cutoff:
                buf.pop(0)
            xs = [t-now for t, _ in buf]
            ys = [v for _, v in buf]
            line.set_data(xs, ys)
            ax.relim()
            ax.autoscale_view()
            return line,

        anim = FuncAnimation(fig, update, interval=50, cache_frame_data=False)
        plt.show()
        return anim

