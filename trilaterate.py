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


#["uplink_message"]["decoded_payload"]["text"]

r1, r2, r3, r4 = 1,1,1,1

def distance(input):
    return 10**(input/56)

def error(input):
    return math.log10(math.pow(input, 56))  

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

def on_subscribe(client, userdata, flags, rc):
    print("Subscribed")

def on_message(client, userdata, msg):
    print(msg.topic)
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005ae68/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            print(1, t)
            recv1(abs(int(t)^303030))
            print("recv1: ", r1)
        except:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d0/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv2(abs(int(t)^303030))
            print(2, t)
            print("recv2: ", r2)
        except KeyError:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d3/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv3(abs(int(t)^303030))
            print(3, t)
            print("recv3: ", r3)
        except KeyError:
            pass
        
    if (msg.topic == "v3/lora-is-pain@ttn/devices/eui-70b3d57ed005c1d8/up"):
        try:
            t = json.loads(msg.payload.decode())["uplink_message"]["decoded_payload"]["text"]
            t = re.sub(r'\D', '', t)
            recv4(abs(int(t)^303030))
            print(4, t)
            print("recv4: ", r4)
        except KeyError:
            pass

def calc(x, y, z, q):
    arr = [Circle(145, 200, x),  
            Circle(145, 80, y),  
            Circle(80, 80, z),
            Circle(80, 200, q)]  
    
    circle, meta = easy_least_squares(arr)  
    #print("result: ", circle)
    return circle.center, circle.radius

def plot_cont(fun, xmax):
    y = []
    fig = plt.figure()
    ax = fig.add_subplot()

    def update(i):
        a,b = fun(r1, r2, r3, r4)  
        y.append((a,b))
        
        ax.clear()
        ax.set_xlim(0,300)
        ax.set_ylim(0,300)
        ax.add_artist(plt.Circle((210, 200), r1, fill=False, color="red"))
        ax.add_artist(plt.Circle((210, 80), r2,
                                 fill=False, color="blue"))
        ax.add_artist(plt.Circle((80, 80), r3, fill=False, color="green"))
        ax.add_artist(plt.Circle((80, 200), r4, fill=False, color="teal"))
        ax.scatter(210,200, s=5,  color="red")
        ax.scatter(210,80, s=5,  color="blue")
        ax.scatter(80,80, s=5,  color="green")
        ax.scatter(80,200, s=5,  color="teal")

        distTxt = "x: {x:.2f}, y: {y:.2f} error: {e:.2f}"
        
        ax.add_artist(plt.Circle((a.x, a.y), b, fill=False)) 
        ax.scatter(a.x,a.y, s=5, color="black", label=distTxt.format(x=distance(a.x-80), y=distance(a.y-80), e=distance(b*2)))
        
        
        plt.legend()
        #print(a,b)
        #print(i, ': ', y[i])

    a = anim.FuncAnimation(fig, update, frames=xmax, repeat=False)
    plt.show()

host = "au1.cloud.thethings.network" # broker
port = 1883 # broker port
username = "lora-is-pain@ttn"
password = "NNSXS.5RLVQBBLKUBUN5K64C3GY2LIUB4N3L4RRCVXO2Y.RMBXZH6PVDOQBYDYXRGEOSS5LJIHG25ZD3CVY43BYSXNXM62A5JA"
client_id = "lkju2aehdglasdaskuahflgiohk" # unique client id string used to connect to broker

client = mqtt.Client(client_id=client_id) # client object / constructor
client.username_pw_set(username=username, password=password) # add inside client object

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
        
