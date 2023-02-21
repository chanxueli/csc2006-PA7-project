from easy_trilateration.model import *  
from easy_trilateration.least_squares import *
from easy_trilateration.graph import *  
import random

def calc(x, y, z):
    arr = [Circle(100, 100, x),  
    Circle(100, 50, y),  
    Circle(50, 50, z)]  
    result, meta = easy_least_squares(arr)  
    print("result: ", result)
    return result

arrRes = []

n = 100
for i in range(n):
    arrRes.append(
        calc(random.randint(20,90), random.randint(20,90), random.randint(20,90))
        )
print(arrRes)

# print(result)
# draw(arr)
