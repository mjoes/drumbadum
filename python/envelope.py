import math
import matplotlib.pyplot as plt
import numpy as np

time = np.linspace(0, 0.1, 10000)
f1 = 39
f0 = 85
interval = 0.05

# c = (f1 - f0) / interval
# x1_t = math.sin(2 * math.pi * ((c * interval * interval / 2) + f0 * interval))

# t_intersect = math.asin(x1_t) / (2 * math.pi * f1) 
# phase_offset = 2 * math.pi * f1 * t_intersect

out = []
for t in time:
    k = pow((f1 / f0), (t / interval))
    if k != 1:
        x = math.sin(2 * math.pi * f0 * (pow(k, (t / interval)) - 1) / math.log(k))
    else:
        x = math.sin(2 * math.pi * f0 * t / interval)  # Handle special case when k = 1
    out.append(x)

plt.plot(time, out)
plt.grid(True)
plt.savefig('y_x_plot.png')


    