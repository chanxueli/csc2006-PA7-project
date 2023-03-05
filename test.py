from easy_trilateration.model import *  
from easy_trilateration.least_squares import *
from easy_trilateration.graph import *  
import paho.mqtt.client as mqtt
import time
# import random
# import paho.mqtt.publish as publish

recv1 = 0
recv2 = 0
recv3 = 0

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
        recv1 = msg.payload.decode()
        print("recv1: ", recv1)
    elif (msg.topic == "Receiver/2"):
        recv2 = msg.payload.decode()
        print("recv2: ", recv2)
    elif (msg.topic == "Receiver/3"):
        recv3 = msg.payload.decode()
        print("recv3: ", recv3)
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def calc(x, y, z):
    arr = [Circle(100, 100, x),  
    Circle(100, 50, y),  
    Circle(50, 50, z)]  
    result, meta = easy_least_squares(arr)  
    print("result: ", result)
    return result

host = "1659bd2762a749d79c6f9453c56635d3.s2.eu.hivemq.cloud" # broker
port = 8883 # broker port
username = "test1"
password = "testtest"
client_id = "lkju2aehdglkuahflgiohk" # unique client id string used to connect to broker
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

while (recv1 != 0 and recv2 != 0 and recv3 != 0):
    calc(recv1, recv2, recv3)
    time.sleep(5)

# arrRes = []

# n = 100
# for i in range(n):
#     arrRes.append(
#         calc(random.randint(20,90), random.randint(20,90), random.randint(20,90))
#         )
# print(arrRes)

# draw(arrRes)