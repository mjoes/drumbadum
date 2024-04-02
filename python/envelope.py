import math
import matplotlib.pyplot as plt
import numpy as np

time = np.linspace(0, 0.1, 10000)
f0 = 78
f1 = 50
interval = 0.06

y_1 = math.sin(2 * math.pi * f0 * interval)
# y_2 = math.sin(math.pi - 2 * math.pi * f0 * interval)
dy_1 = 2 * math.pi * f0 * math.cos(2 * math.pi * f0 * interval)
# dy_2 = 2 * math.pi * f0 * math.cos(math.pi - 2 * math.pi * f0 * interval)
sign_1 = math.copysign(1,dy_1)

phi_1 = math.asin(y_1)# - (2 * math.pi * f1 * interval)
phi_2 = math.pi - phi_1
dz_1 = 2 * math.pi * f1 * math.cos(phi_1)
dz_2 = 2 * math.pi * f1 * math.cos(phi_2)
while phi_1 < 0:
    phi_1 = phi_1 + 2*math.pi

while phi_2 > 2 * math.pi:
    phi_2 = phi_2 - 2 * math.pi
if math.copysign(1, dz_1) == sign_1:
    phi = phi_1
else:
    phi = phi_2
print(y_1, dy_1, sign_1)
print(phi_1, phi_2)
print(dz_1, dz_2)

out = []
for t in time:
    if t < interval:
        y = math.sin(2 * math.pi * f0 * t)
    else:
        y = math.sin(2 * math.pi * f1 * (t - interval) + phi )
    out.append(y)

plt.plot(time, out)
plt.grid(True)
plt.savefig('y_x_plot.png')


    