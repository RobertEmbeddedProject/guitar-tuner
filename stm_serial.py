import sys
import struct
from collections import deque

import numpy as np
import serial
import pyqtgraph as pg
from PyQt6 import QtCore, QtWidgets

PORT = "COM6"
BAUD = 460800

CHUNK_SAMPLES = 32
SAMPLE_RATE = 8000.0
DISPLAY_SAMPLES = 1024  # 128 ms rolling window

SYNC = b"\xA5\x5A\xA5\x5A"
PAYLOAD_LEN = CHUNK_SAMPLES * 2


def checksum16(data: bytes) -> int:
    return sum(data) & 0xFFFF


class SerialPlotter(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("STM32 Guitar ADC Viewer")

        self.ser = serial.Serial(PORT, BAUD, timeout=0.0)
        self.ser.reset_input_buffer()

        self.buf = deque([2048] * DISPLAY_SAMPLES, maxlen=DISPLAY_SAMPLES)
        self.x_ms = np.arange(DISPLAY_SAMPLES, dtype=np.float64) * (1000.0 / SAMPLE_RATE)

        self.good_packets = 0
        self.bad_packets = 0

        self.plot_widget = pg.PlotWidget()
        self.setCentralWidget(self.plot_widget)

        self.plot_widget.setLabel("bottom", "Time", units="ms")
        self.plot_widget.setLabel("left", "ADC Counts")
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setXRange(0, self.x_ms[-1], padding=0)
        self.plot_widget.setYRange(1700, 2400, padding=0)

        self.curve = self.plot_widget.plot(
            self.x_ms,
            np.array(self.buf, dtype=np.float64),
            pen=pg.mkPen(width=1)
        )

        # Faster updates than matplotlib
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(15)   # about 66 FPS target

        self.status_timer = QtCore.QTimer()
        self.status_timer.timeout.connect(self.update_title)
        self.status_timer.start(250)

    def closeEvent(self, event):
        try:
            if self.ser.is_open:
                self.ser.close()
        finally:
            event.accept()

    def read_exact(self, n: int, max_loops: int = 200):
        data = bytearray()
        loops = 0
        while len(data) < n and loops < max_loops:
            chunk = self.ser.read(n - len(data))
            if chunk:
                data.extend(chunk)
            else:
                loops += 1
        return bytes(data) if len(data) == n else None

    def find_sync(self):
        window = bytearray()
        max_loops = 500
        loops = 0

        while loops < max_loops:
            b = self.ser.read(1)
            if not b:
                loops += 1
                continue

            window += b
            if len(window) > 4:
                window = window[-4:]

            if bytes(window) == SYNC:
                return True

        return False

    def read_packet(self):
        if not self.find_sync():
            return None

        hdr = self.read_exact(4)
        if hdr is None:
            self.bad_packets += 1
            return None

        chunk_id = hdr[0]
        seq = hdr[1]
        payload_len = hdr[2] | (hdr[3] << 8)

        if payload_len != PAYLOAD_LEN:
            self.bad_packets += 1
            return None

        payload = self.read_exact(payload_len)
        if payload is None:
            self.bad_packets += 1
            return None

        tail = self.read_exact(2)
        if tail is None:
            self.bad_packets += 1
            return None

        rx_csum = tail[0] | (tail[1] << 8)
        calc_csum = checksum16(payload)

        if rx_csum != calc_csum:
            self.bad_packets += 1
            return None

        self.good_packets += 1
        samples = struct.unpack("<" + "H" * CHUNK_SAMPLES, payload)
        return chunk_id, seq, samples

    def update_plot(self):
        # Drain several packets per GUI tick so the display stays current
        packets_this_tick = 0
        max_packets_per_tick = 16

        while packets_this_tick < max_packets_per_tick:
            pkt = self.read_packet()
            if pkt is None:
                break

            _, _, samples = pkt
            self.buf.extend(samples)
            packets_this_tick += 1

        if packets_this_tick > 0:
            y = np.fromiter(self.buf, dtype=np.float64, count=DISPLAY_SAMPLES)
            self.curve.setData(self.x_ms, y)

    def update_title(self):
        self.setWindowTitle(
            f"STM32 Guitar ADC Viewer | good={self.good_packets} bad={self.bad_packets}"
        )


def main():
    app = QtWidgets.QApplication(sys.argv)
    pg.setConfigOptions(antialias=False)

    window = SerialPlotter()
    window.resize(1100, 600)
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()