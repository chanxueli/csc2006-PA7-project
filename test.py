from easy_trilateration.model import *  
from easy_trilateration.least_squares import *
from easy_trilateration.graph import *  
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import numpy as np
import paho.mqtt.client as mqtt
import time

r1, r2, r3 = 50,50,50
def recv1(input):
    global r1
    r1 = input
    
def recv2(input):
    global r2
    r2 = input
    
def recv3(input):
    global r3
    r3 = input

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Receiver/#")

def on_subscribe(client, userdata, flags, rc):
    print("Subscribed")

def on_message(client, userdata, msg):
    if (msg.topic == "Receiver/1"):
        recv1(abs(int(msg.payload.decode())))
        print("recv1: ", r1)
    elif (msg.topic == "Receiver/2"):
        recv2(abs(int(msg.payload.decode())))
        print("recv2: ", r2)
    elif (msg.topic == "Receiver/3"):
        recv3(abs(int(msg.payload.decode())))
        print("recv3: ", r3)

def calc(x, y, z):
    arr = [Circle(100, 100, x),  
    Circle(100, 50, y),  
    Circle(50, 50, z)]  
    circle, meta = easy_least_squares(arr)  
    print("result: ", circle)
    return circle.center, circle.radius

def plot_cont(fun, xmax):
    y = []
    fig = plt.figure()
    ax = fig.add_subplot()

    def update(i):
        a,b = fun(r1, r2, r3)  
        y.append((a,b))
        
        ax.clear()
        ax.set_xlim(0,200)
        ax.set_ylim(0,200)
        ax.add_artist(plt.Circle((100, 100), r1, fill=False, color="red"))
        ax.add_artist(plt.Circle((100, 50), r2, fill=False, color="blue"))
        ax.add_artist(plt.Circle((50, 50), r3, fill=False, color="green"))
        ax.scatter(100,100, s=5,  color="red")
        ax.scatter(100,50, s=5,  color="blue")
        ax.scatter(50,50, s=5,  color="green")

        ax.add_artist(plt.Circle((a.x, a.y), b, fill=False))
        ax.scatter(a.x,a.y, s=5)
        print(a,b)
        #print(i, ': ', y[i])

    a = anim.FuncAnimation(fig, update, frames=xmax, repeat=False)
    plt.show()

host = "1659bd2762a749d79c6f9453c56635d3.s2.eu.hivemq.cloud" # broker
port = 8883 # broker port
username = "test2"
password = "testtest"
client_id = "lkju2aehdglasdaskuahflgiohk" # unique client id string used to connect to broker
topic = "receviver/1"

client = mqtt.Client(client_id=client_id) # client object / constructor
client.username_pw_set(username=username, password=password) # add inside client object
client.tls_set()

client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message

client.connect(host, port=port,)
client.subscribe("receiver/#", qos=0)
client.loop_start()

while (1):
    print(r1, r2, r3)
    while (r1 != 0 and r2 != 0 and r3 != 0):
        plot_cont(calc, 321231)
        
