from easy_trilateration.model import *  
from easy_trilateration.least_squares import *
from easy_trilateration.graph import *  
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import numpy as np
import paho.mqtt.client as mqtt
import time
import math
import json
import re

#Init RSSI Values
r1, r2, r3, r4 = 1,1,1,1

#MQTT Settings
host = "au1.cloud.thethings.network" # broker
port = 1883 # broker port
username = "lora-is-pain@ttn"
password = "NNSXS.5RLVQBBLKUBUN5K64C3GY2LIUB4N3L4RRCVXO2Y.RMBXZH6PVDOQBYDYXRGEOSS5LJIHG25ZD3CVY43BYSXNXM62A5JA"
client_id = "lkju2aehdglasdaskuahflgiohk" # unique client id string used to connect to broker

def distance(input):
    if input == 0:
        return 1
    return math.exp(5.1*((input/70) - 1))

def recv1(input):
    global r1
    r1 = input
    
def recv2(input):
    global r2
    r2 = input
    
def recv3(input):
    global r3
    r3 = input

def recv4(input):
    global r4
    r4 = input
    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("v3/lora-is-pain@ttn/devices/#")

# Successful Subscribe callback
def on_subscribe(client, userdata, flags, rc):
    print("Subscribed")

# Decodes the message and assign it to the respective variables upon receiving a message.
def on_message(client, userdata, msg):
    print(msg.topic)
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005ae68/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            print(1, int(t)^303030)
            recv1(distance(abs(int(t)^303030)))
            print("recv1: ", r1)
        except:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d0/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv2(distance(abs(int(t)^303030)))
            print(2, int(t)^303030)
            print("recv2: ", r2)
        except KeyError:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d3/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv3(distance(abs(int(t)^303030)))
            print(3, int(t)^303030)
            print("recv3: ", r3)
        except KeyError:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d8/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv4(distance(abs(int(t)^303030)))
            print(4, int(t)^303030)
            print("recv4: ", r4)
        except KeyError:
            pass

# Compute the estimated location
def calc(x, y, z, q):
    arr = [Circle(4, 4, x),  
            Circle(4, 0, y),  
            Circle(0, 0, z),
            Circle(0, 4, q)]  
    
    circle, meta = easy_least_squares(arr)  
    #print("result: ", circle)
    return circle.center, circle.radius

#Draw out the plot on a window.
def plot_cont(fun, xmax):
    y = []
    fig = plt.figure()
    ax = fig.add_subplot()

    def update(i):
        a,b = fun(r1, r2, r3, r4)  
        y.append((a,b))
        
        ax.clear()
        ax.set_xlim(-2,6)
        ax.set_ylim(-2,6)
        ax.add_artist(plt.Circle((4, 4), float(r1), fill=False, color="red"))
        ax.add_artist(plt.Circle((4, 0), float(r2), fill=False, color="blue"))
        ax.add_artist(plt.Circle((0, 0), float(r3), fill=False, color="green"))
        ax.add_artist(plt.Circle((0, 4), float(r4), fill=False, color="teal"))
        ax.scatter(4,4, s=1,  color="red")
        ax.scatter(4,0, s=1,  color="blue")
        ax.scatter(0,0, s=1,  color="green")
        ax.scatter(0,4, s=1,  color="teal")

        distTxt = "x: {x:.2f}, y: {y:.2f} error: {e:.2f}"
        
        ax.add_artist(plt.Circle((a.x, a.y), b, fill=False)) 
        ax.scatter(a.x,a.y, s=1, color="black", label=distTxt.format(x=a.x, y=a.y, e=b*2))
        
        
        plt.legend()

    a = anim.FuncAnimation(fig, update, frames=xmax, repeat=False)
    plt.show()

client = mqtt.Client(client_id=client_id) # client object / constructor
client.username_pw_set(username=username, password=password) # add inside client object

#Assign callbacks
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message

client.connect(host, port=port,)
client.subscribe("v3/lora-is-pain@ttn/devices/#", qos=0)
client.loop_start()

while (1):
    #print(r1, r2, r3)
    while (r1 != 0 and r2 != 0 and r3 != 0 and r4 != 0):
        plot_cont(calc, 321231)
        
