import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.animation import PillowWriter

points = [(0.1, 0.5), (0.5, 0.5), (0.9, 0.5)]
 
fig, ax = plt.subplots(1, 1)
fig.set_size_inches(5,5)
 
def animate(i):
    ax.clear()
    # Get the point from the points list at index i
    point = points[i]
    # Plot that point using the x and y coordinates
    ax.plot(point[0], point[1], color='green', 
            label='original', marker='o')
    # Set the x and y axis to display a fixed range
    ax.set_xlim([0, 1])
    ax.set_ylim([0, 1])
ani = FuncAnimation(fig, animate, frames=len(points),
                    interval=500, repeat=False)
ani.save("simple_animation.gif", dpi=300,
         writer=PillowWriter(fps=1))
plt.close()