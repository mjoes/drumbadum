import matplotlib.pyplot as plt
import numpy as np
from math import tan, pi


def env(x):
    y = 32767 * 1.5 * (x - x*x*x / 3)
    return round(y)

x = np.linspace(-1, 1, 256).tolist()
y = list(map(env, x))

num_columns = 4  # Number of columns in the output text file
y_reshaped = np.reshape(y, (-1, num_columns))

with open('y_values.txt', 'w') as file:
    for row in y_reshaped:
        row_str = ", ".join(map(str, row))
        file.write(f"{row_str},\n")

plt.plot(x, y)
plt.grid(True)
plt.savefig('y_x_plot.png')