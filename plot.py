import matplotlib.pyplot as plt
import matplotlib
import numpy as np

# Generate some random data
y= [40, 40, 40, 40, 40, 40, 40, 30, 30, 30, 30, 30, 30, 30, 30, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 10, 10, 10, 10, 10, 10, 10, 0.5, 0.5, 0.5, 1, 1, 1, 1, 5, 5, 5, 5, 5
]

x =[85, 86, 85, 84, 82, 86, 87, 84, 79, 79, 80, 80, 78, 78, 78, 77, 77, 77, 76, 79, 77, 75, 77, 74, 74, 79, 78, 78, 72, 74, 71, 72, 72, 71, 72, 7, 7, 7, 8, 9, 9, 8, 56, 56, 57, 55, 54
]

# Create the scatterplot


scale =  np.ones(len(x)) * 3

xfit =  np.linspace(0,120,1200)
yfit = 10**(xfit/57)
plt.scatter(x, y, s=scale, c='red', label="Data Collected")
# Add labels and title
plt.xlabel('RSSI readings')
plt.ylabel('Distance')
plt.title('RSSI-Distance Map')
plt.xlim(0,130)
plt.ylim(0,100)
plt.plot(xfit,yfit, label="Distance as per Formula; n=5.6")
plt.legend()
# Show the plot
plt.show()
