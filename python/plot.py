import matplotlib.pyplot as plt
import numpy as np
import math

def env(x):
    y = -x**0.1
    y = (y+1) * 65535
    return round(y)

def env2(x):
    y = math.exp(-42*x)
    y = (y) * 65535
    return round(y)


x = np.linspace(0, 0.28125, 256).tolist()
y = list(map(env2, x))

num_columns = 4  # Number of columns in the output text file
y_reshaped = np.reshape(y, (-1, num_columns))

with open('y_values.txt', 'w') as file:
    for row in y_reshaped:
        row_str = ", ".join(map(str, row))
        file.write(f"{row_str},\n")

plt.plot(x, y)
plt.grid(True)
plt.savefig('y_x_plot.png')