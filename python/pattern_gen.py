import numpy as np
import math
import random 

val = []
for i in range(0,752):
    val.append(random.randint(0,100))

num_columns = 4  # Number of columns in the output text file
val_reshaped = np.reshape(val, (-1, num_columns))

with open('pattern.txt', 'w') as file:
    for row in val_reshaped:
        row_str = ", ".join(map(str, row))
        file.write(f"{row_str},\n")