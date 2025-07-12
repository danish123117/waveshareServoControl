
from math import *
import matplotlib.pyplot as plt
import numpy as np


L1 = 0.17
L2 = 0.17
L3 = 0.17
L4 = 0.17
L5 = 0.1

valid_x3= []
valid_y3 =[]
t1= np.linspace(0, 2 * np.pi, 360)  # Generate 100 points from 0 to 2π
t5 = np.linspace(0, 2 * np.pi, 360)  # Generate 100 points from 0 to 2π  
 
for t1_val in t1:
    for t5_val in t5:
        x2 = L1 * np.cos( t1_val )
        y2 = L1 * np.sin( t1_val )
        x4 = L4 * np.cos( t5_val ) - L5
        y4 = L4 * np.sin( t5_val )
        P4P2 = sqrt(( x4 - x2) ** 2 + (y4 - y2) ** 2)
        PHP2 = (L2 ** 2 - L3 ** 2 + P4P2 ** 2) / (2 * P4P2 )
        if P4P2 <= L2 or (L2 ** 2 - PHP2 ** 2) < 0:
            continue
        PHP3 = sqrt ( max (L2 ** 2 - PHP2 ** 2, 0))
        xH = x2 + PHP2 / P4P2 * (x4 - x2)
        yH = y2 + PHP2 / P4P2 * (y4 - y2)
        x3 = xH + PHP3 / P4P2 * (y4 - y2)
        y3 = yH - PHP3 / P4P2 * (x4 - x2)
        valid_x3.append(x3)
        valid_y3.append(y3)

plt.scatter(valid_x3,valid_x3)
plt.show()