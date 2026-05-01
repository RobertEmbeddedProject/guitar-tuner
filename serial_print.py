import sys
import struct
from pathlib import Path
from datetime import datetime

import serial
import numpy as np
import pyqtgraph as pg
from PyQt6 import QtCore, QtWidgets

## to run python:
## python serial_print.py

## Press R to start recording, then R again to save a recording to /Samples

## in Powershell, use this to activate serial_view.py:
## python serial_view.py Samples/capture_20260426_135155.npy

PORT = "COM7"
BAUD = 460800

CHUNK_SAMPLES = 32
SAMPLE_RATE = 20000.0
DISPLAY_SAMPLES = 64000

SYNC = b"\xA5\x5A\xA5\x5A"
PAYLOAD_LEN = CHUNK_SAMPLES * 2


def checksum16(data: bytes) -> int:
    return sum(data) & 0xFFFF


class SerialScope(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        self.script_dir = Path(__file__).resolve().parent
        self.samples_dir = self.script_dir / "Samples"
        self.samples_dir.mkdir(exist_ok=True)

        self.recording = False
        self.record_buffer = []
        self.record_name = None

        self.ser = serial.Serial(PORT, BAUD, timeout=0.0)
        self.ser.reset_input_buffer()

        self.good_packets = 0
        self.bad_packets = 0
        self.last_seq = None

        self.y = np.full(DISPLAY_SAMPLES, 2048.0, dtype=np.float64)
        self.x_ms = np.arange(DISPLAY_SAMPLES, dtype=np.float64) * (1000.0 / SAMPLE_RATE)

        self.plot_widget = pg.PlotWidget()
        self.setCentralWidget(self.plot_widget)

        self.plot_widget.setLabel("bottom", "Time", units="ms")
        self.plot_widget.setLabel("left", "ADC Counts")
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setXRange(0, self.x_ms[-1], padding=0)
        self.plot_widget.setYRange(1700, 2400, padding=0)

        self.curve = self.plot_widget.plot(self.x_ms, self.y, pen=pg.mkPen(width=1))

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.poll_serial)
        self.timer.start(5)

        self.status_timer = QtCore.QTimer()
        self.status_timer.timeout.connect(self.update_title)
        self.status_timer.start(250)

    def keyPressEvent(self, event):
        key = event.key()

        if key == QtCore.Qt.Key.Key_R:
            if self.recording:
                self.stop_recording()
            else:
                self.start_recording()

        elif key == QtCore.Qt.Key.Key_S:
            self.save_capture()

    def start_recording(self):
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.record_name = f"capture_{timestamp}"
        self.record_buffer = []
        self.recording = True
        print(f"Recording started: {self.record_name}")

    def stop_recording(self):
        self.recording = False
        self.save_capture()
        print("Recording stopped")

    def save_capture(self):
        if not self.record_buffer:
            print("No samples to save")
            return

        arr = np.array(self.record_buffer, dtype=np.uint16)

        npy_path = self.samples_dir / f"{self.record_name}.npy"
        csv_path = self.samples_dir / f"{self.record_name}.csv"

        np.save(npy_path, arr)
        np.savetxt(csv_path, arr, fmt="%u", delimiter=",")

        print(f"Saved {len(arr)} samples")
        print(f"NPY: {npy_path}")
        print(f"CSV: {csv_path}")

    def closeEvent(self, event):
        try:
            if self.recording:
                self.stop_recording()
            if self.ser.is_open:
                self.ser.close()
        finally:
            event.accept()

    def read_exact(self, n: int, max_spins: int = 1000):
        data = bytearray()
        spins = 0
        while len(data) < n and spins < max_spins:
            chunk = self.ser.read(n - len(data))
            if chunk:
                data.extend(chunk)
            else:
                spins += 1
        if len(data) == n:
            return bytes(data)
        return None

    def find_sync(self):
        window = bytearray()
        spins = 0
        max_spins = 4000

        while spins < max_spins:
            b = self.ser.read(1)
            if not b:
                spins += 1
                continue

            window += b
            if len(window) > 4:
                window = window[-4:]

            if bytes(window) == SYNC:
                return True

        return False

    def read_one_packet(self):
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

        self.last_seq = seq
        self.good_packets += 1

        samples = struct.unpack("<" + "H" * CHUNK_SAMPLES, payload)
        return chunk_id, seq, samples

    def poll_serial(self):
        updated = False
        max_packets_per_tick = 32

        for _ in range(max_packets_per_tick):
            pkt = self.read_one_packet()
            if pkt is None:
                break

            _, _, samples = pkt
            n = len(samples)

            self.y[:-n] = self.y[n:]
            self.y[-n:] = samples

            if self.recording:
                self.record_buffer.extend(samples)

            updated = True

        if updated:
            self.curve.setData(self.x_ms, self.y)

    def update_title(self):
        rec = "REC" if self.recording else "idle"
        count = len(self.record_buffer)
        self.setWindowTitle(
            f"STM32 Guitar ADC Viewer | good={self.good_packets} bad={self.bad_packets} | {rec} samples={count}"
        )


def main():
    app = QtWidgets.QApplication(sys.argv)
    pg.setConfigOptions(antialias=False)

    window = SerialScope()
    window.resize(1200, 650)
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()