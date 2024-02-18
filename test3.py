from math import pi, sin, log, exp
import matplotlib.pyplot as plt
import numpy as np

def chirp(t, f0, f1, slope, interval, phase_end):
    if t > interval:
        func = f1*t
        return np.sin(2*np.pi*func + phase_end)
    else:
        func = slope*t**2/2+f0*t
        return np.sin(2*np.pi*func)

# for a visually useful example, take:
timeLength = 3
interval = 0.5 # seconds
fs = 44100 # sampling rates
t = np.arange(0, timeLength, 1.0/fs)
f1 = 5
f0 = 16

slope = (f1-f0)/interval
phase_end = -2 * np.pi * (f0 + f1) / 2 * interval

chirp_out = [chirp(x, f0, f1, slope, interval, phase_end) for x in t]

plt.plot(t,chirp_out)
plt.savefig('test3.png') 