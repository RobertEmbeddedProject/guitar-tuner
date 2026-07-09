import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

SAMPLE_RATE = 20000.0

def analyze(file):
    data = np.load(file).astype(np.float32)

    print(f"Samples: {len(data)}")
    print(f"Min: {np.min(data):.0f}")
    print(f"Max: {np.max(data):.0f}")
    print(f"Mean/DC offset: {np.mean(data):.2f}")

    data -= np.mean(data)

    fft = np.fft.rfft(data)
    freqs = np.fft.rfftfreq(len(data), d=1.0 / SAMPLE_RATE)

    mag = np.abs(fft)
    mag[0] = 0  # ignore DC

    peak = freqs[np.argmax(mag)]
    print(f"Detected FFT peak: {peak:.2f} Hz")

    PLOT_SAMPLES = min(64000, len(data))
    
    plt.figure()
    plt.plot(data[:PLOT_SAMPLES], linewidth=0.35)
    plt.title("Time-domain waveform")
    plt.xlabel("Sample")
    plt.ylabel("ADC counts, centered")
    plt.grid(True)

    ZOOM_SAMPLES = min(2000, len(data))

    plt.figure()
    plt.plot(data[:ZOOM_SAMPLES], linewidth=0.6)
    plt.title("Time-domain waveform zoom")
    plt.xlabel("Sample")
    plt.ylabel("ADC counts, centered")
    plt.grid(True)

    plt.figure()
    plt.plot(freqs, mag)
    plt.title("Frequency spectrum")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.xlim(0, 1000)
    
    ax = plt.gca()
    ax.xaxis.set_major_locator(ticker.MultipleLocator(50))

    plt.xticks(rotation=45)   # or 30, 60 depending on preference

    plt.grid(True)

    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage:")
        print("  python analyze_sample.py Samples/1_Low_E.npy")
        sys.exit(1)

    analyze(sys.argv[1])