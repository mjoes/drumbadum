# UNUSED AT THE MOMENT, BUT LEAVING IT HERE FOR NOW
# CAN BE USED TO CREATE A WHITE NOISE HIT SAMPLE
import matplotlib.pyplot as plt
import numpy as np
import math
import random

duration = 0.08
sample_rate = 48000
nr_samples = int(duration * sample_rate)
rate = -math.log(1e-3) / duration

x = range(0,nr_samples)
y = []
for i in range(0,nr_samples):
    t = i/sample_rate
    y_t = np.exp(-rate * t) * random.uniform(-1,1) * 32767
    y.append(round(y_t))

plt.plot(x, y)
plt.grid(True)
plt.savefig('y_x_plot.png')

num_columns = 4  # Number of columns in the output text file
y_reshaped = np.reshape(y, (-1, num_columns))

with open('y_values.txt', 'w') as file:
    for row in y_reshaped:
        row_str = ", ".join(map(str, row))
        file.write(f"{row_str},\n")