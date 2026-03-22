import serial
import numpy as np
from scipy.signal import butter, filtfilt
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# Configuration
PORT = 'COM9'
BAUD = 115200
SAMPLE_RATE = 25
BUFFER_SIZE = 250  # 10 seconds of data

# Butterworth bandpass filter 0.5-4 Hz
def butter_bandpass(lowcut=0.5, highcut=4.0, fs=25, order=2):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

b, a = butter_bandpass()

# Data buffer
raw_buffer = deque([0] * BUFFER_SIZE, maxlen=BUFFER_SIZE)
filtered_buffer = deque([0] * BUFFER_SIZE, maxlen=BUFFER_SIZE)

# Setup plot
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6))
fig.suptitle('VitalGuard — Live PPG Signal', fontsize=14)

ax1.set_title('Raw Signal')
ax1.set_ylim(0, 65535)
ax1.set_ylabel('IR Value')
line1, = ax1.plot([], [], 'b-', linewidth=1)

ax2.set_title('Filtered Signal (Bandpass 0.5-4Hz)')
ax2.set_ylabel('Amplitude')
line2, = ax2.plot([], [], 'g-', linewidth=1.5)

# Connect to ESP32
try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"Connected to {PORT}")
except Exception as e:
    print(f"Cannot connect to {PORT}. Error: {e}")
    input("Press Enter to exit...")
    exit()

def update(frame):
    # Read all available lines
    while ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line.isdigit():
                raw_buffer.append(int(line))
        except:
            pass

    raw_array = np.array(raw_buffer, dtype=float)

    # Apply bandpass filter
    if len(raw_array) >= BUFFER_SIZE:
        try:
            filtered = filtfilt(b, a, raw_array)
            filtered_buffer.extend(filtered)
        except:
            filtered = raw_array

    # Update plots
    x = np.arange(BUFFER_SIZE)
    line1.set_data(x, raw_array)
    line2.set_data(x, np.array(filtered_buffer))

    ax1.set_xlim(0, BUFFER_SIZE)
    ax2.set_xlim(0, BUFFER_SIZE)
    ax2.relim()
    ax2.autoscale_view()

    return line1, line2

ani = animation.FuncAnimation(fig, update, interval=40, blit=True)
plt.tight_layout()
plt.show()

ser.close()
input("Press Enter to exit...")

