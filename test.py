from easy_trilateration.model import *  
from easy_trilateration.least_squares import *
from easy_trilateration.graph import *  
import random
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Receiver/#")

def on_subscribe(client, userdata, flags, rc):
    print("Subscribed")

def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def calc(x, y, z):
    arr = [Circle(100, 100, x),  
    Circle(100, 50, y),  
    Circle(50, 50, z)]  
    result, meta = easy_least_squares(arr)  
    print("result: ", result)
    return result

host = "1659bd2762a749d79c6f9453c56635d3.s2.eu.hivemq.cloud" # broker
port = 8883
username = "test1"
password = "testtest"
client_id = "lkju2aehdglkuahflgiohk" # unique client id string used to connect to broker
topic = "receviver/1"

client = mqtt.Client(client_id=client_id)
client.username_pw_set(username=username, password=password) # add inside client object
client.tls_set()

client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message

client.connect(host, port=port,)
client.subscribe("receiver/#", qos=0)
client.loop_start()

while True:
    time.sleep(3)

# arrRes = []

# n = 100
# for i in range(n):
#     arrRes.append(
#         calc(random.randint(20,90), random.randint(20,90), random.randint(20,90))
#         )
# print(arrRes)

# draw(arrRes)