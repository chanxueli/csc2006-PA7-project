# Create a client instance

# Connect to a broker using one of the connect*() functions

# Call one of the loop*() functions to maintain network traffic flow with the broker

# Use subscribe() to subscribe to a topic and receive messages

# Use publish() to publish messages to the broker

# Use disconnect() to disconnect from the broker

# Callbacks will be called to allow the application to process events as necessary. These callbacks are described below.

import paho.mqtt.client as mqtt

# called when broker responds to connection request
def on_connect(mqttc, userdata, flags, rc):
    if userdata == 0:
        print("First connection:")
    elif userdata == 1:
        print("Second connection:")
    elif userdata == 2:
        print("Third connection (with clean session=True):")
    print("    Session present: " + str(flags['session present']))
    print("    Connection result: " + str(rc))
    mqttc.disconnect()

# called when client disconnects from broker
def on_disconnect(mqttc, userdata, rc):
    mqttc.user_data_set(userdata + 1)
    if userdata == 0:
        mqttc.reconnect()
    elif rc != 0:
        print("Unexpected disconnection.")

def on_log(mqttc, userdata, level, string):
    print(string)

# client Constructor
mqttc = mqtt.Client(client_id="asdfj", clean_session=False)
# clean_session - True: broker remove all info about client when disconnect
# False - subscribed info and queued msgs will be retained when client disconnects
mqttc.on_connect = on_connect
mqttc.on_disconnect = on_disconnect

# Uncomment to enable debug messages
# mqttc.on_log = on_log
mqttc.user_data_set(0)
mqttc.connect("1659bd2762a749d79c6f9453c56635d3.s2.eu.hivemq.cloud", 8883, 60) # 60 - keepallive (max period in sec allows between comms with broker, controls rate which client ping msg to broker)

mqttc.loop_forever() # check loop documentation

# Clear session
mqttc = mqtt.Client(client_id="asdfj", clean_session=True)
mqttc.on_connect = on_connect
mqttc.user_data_set(2)
mqttc.connect("1659bd2762a749d79c6f9453c56635d3.s2.eu.hivemq.cloud", 8883, 60)
mqttc.loop_forever() # check loop documentation

# reinitialise client to its starting state (no need?)

