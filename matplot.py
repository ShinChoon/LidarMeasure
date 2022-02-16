
# importing the required module

import numpy
import _thread
import re
import zlib
import json
import paho.mqtt.client as mqtt
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

try:
    import msgpack
except ImportError:
    # No msgpack installed, attempt json
    msgpack = json

SUPPORTED_ENCODERS = {
    "json": (json.loads, json.dumps),
    "msgpack": (msgpack.loads, msgpack.dumps),
    "mz": (lambda data: msgpack.loads(zlib.decompress(data)), lambda data: zlib.compress(msgpack.dumps(data)))
}

CoordinateVeector = "X"
CooridnateValue = 0
XValue = None
YValue = None


verts = [
   (1500, 2700),  # left, bottom
   (1500, 1000),  # left, top
   (2500, 1000),  # right, top
   (2500, 2700),  # right, bottom
   (1500, 2500),  # ignored
]

codes = [
    Path.MOVETO,
    Path.LINETO,
    Path.LINETO,
    Path.LINETO,
    Path.CLOSEPOLY,
]

path = Path(verts, codes)

DEFAULT_MQTT_TOPICS = "{username}/{feeds}/{key}"

def update_line(hl, xx,yy):
    global h1
    xdata = hl.get_xdata()
    ydata = hl.get_ydata()

    hl.set_xdata(numpy.append(xdata, xx))
    hl.set_ydata(numpy.append(ydata, yy))
    plt.xlim([10,3000])
    plt.ylim([10,3000])
    ax1 = plt.subplot()
    patch = patches.PathPatch(path, facecolor='orange', lw=2)
    ax1.add_patch(patch)
    ax1.set_xlabel("Terranger / cm")
    ax1.set_ylabel("LiDar / cm")
    if(1500<xx<2500 and 1000<yy<2700):
        # plt.plot(xx, yy, 'ro')
        plt.scatter(xx,yy,color='red')
    else:
        # plt.plot(xx, yy, 'yo')
        plt.scatter(xx,yy,color='blue')
    plt.text(xx, yy+0.5, '({}, {})'.format(xx, yy))
    plt.draw()


def plotShow():
    plt.show()

def UpdatePlot(xx,yy):
    plt.cla()
    xx = 3000-xx
    yy = 3000-yy
    update_line(hl,int(xx),int(yy))

def get_mqtt_auth():
    auth = {
        "username": "ZHONGXIN",
        "password": "aio_ouZi51bMrmVqkZeAZUOCdYiQj7XM",
    }

    return auth      

def decode_message(payload, channel):
    encoder = channel.get("encoder", "json")

    if encoder not in SUPPORTED_ENCODERS:
        raise NotImplementedError(encoder)

    loads, dumps = SUPPORTED_ENCODERS[encoder]

    try:
        payload = loads(payload.decode('utf-8'))
        return payload
    except UnicodeDecodeError:
        print("UnicodeDecodeError")

    return None

def on_message_received(client, userdata, message):
    global CoordinateVeector
    global CooridnateValue
    global XValue, YValue
    # print("@@@@@@@MQTT received {0} {1}".format(
    #     message.topic, message.payload))

    if message.topic == "ZHONGXIN/feeds/x-value":
        CoordinateVeector = "X"
    
    elif message.topic == "ZHONGXIN/feeds/y-value":
        CoordinateVeector = "Y"

    try:
        payload = message.payload
        message = payload.decode("utf-8")
        if message:
            print("receive: ", message)
            CooridnateValue = int(message)

            if CoordinateVeector == "X":
                XValue = CooridnateValue
            elif CoordinateVeector == "Y":
                YValue = CooridnateValue

        if (XValue is not None) and (YValue is not None):
            print("XValue: ", XValue)
            print("YValue: ", YValue)
            UpdatePlot(YValue, XValue)
            XValue = None
            YValue = None
    except UnicodeDecodeError:
        print("UnicodeDecodeError")

def on_subscribe(client, userdata, mid, granted_qos):
    print("MQTT listener connected")

def on_connect(client, userdata, flags, rc):
    topics = [
       (DEFAULT_MQTT_TOPICS.format(username="ZHONGXIN", feeds="feeds", key="x-value"),0),
       (DEFAULT_MQTT_TOPICS.format(username="ZHONGXIN", feeds="feeds", key="y-value"),0)
    ]
    print("MQTT Connecting listener on {0}".format(repr(topics)))
    client.subscribe(topics, qos=1)


def on_disconnect(client, userdata, rc):
    if rc == mqtt.MQTT_ERR_SUCCESS:
        # Client disconnected normally, no action
        print("MQTT client closing connection")
        return
    # auth = get_mqtt_auth()
    # client.username_pw_set(auth["username"], auth["password"])
    client.reconnect()  

# naming the x axis
plt.xlabel('x - axis')
# naming the y axis
plt.ylabel('y - axis')
 
# giving a title to my graph
plt.title('My first graph!')

plt.xlim([10,3000])
plt.ylim([10,3000])
global ax1
ax1 = plt.subplot()
ax1.set_xlabel("Terranger / cm")
ax1.set_ylabel("LiDar / cm")
# plotting the points
hl, = plt.plot(0,0)
 



client = mqtt.Client("P1")
# auth = get_mqtt_auth()
# client.username_pw_set(auth["username"], auth["password"])


client.on_message = on_message_received
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_subscribe = on_subscribe


def useMQTT():
    client.connect(
        "localhost",1883,
        keepalive=True,
    )
    print("start")
    try:
        client.loop_forever()
    except KeyboardInterrupt:
        client.disconnect()
        raise

# function to show the plot
_thread.start_new_thread(useMQTT,())


patch = patches.PathPatch(path, facecolor='orange', lw=2)
ax1.add_patch(patch)
plotShow()



