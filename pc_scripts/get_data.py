from tqdm import tqdm
from time import sleep
import os.path
import struct
import time
import json
import math
import serial
import serial.tools.list_ports
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, lfilter, hilbert
from mpl_toolkits.mplot3d import Axes3D

BUF_SIZE = 5000


def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype="band")
    return b, a


def butter_bandpass_filter(data, lowcut, highcut, fs, order=1):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y


def t(i):
    return ((float(i) / fs)) * 340 / 2


fs = 138888
lowcut = 38000
highcut = 42000


startMarker = 254
endMarker = 255


ports = list(serial.tools.list_ports.comports())
port_emitter = ""
port_receiver = ""
for p in ports:
    print(p.hwid)
    if "SER=8033700" in p.hwid:
        port_emitter = p[0]
    if "SER=1819010" in p.hwid:
        port_receiver = p[0]

print("emitter=", port_emitter, "\treceiver=", port_receiver)
ser_emitter = serial.Serial(port_emitter, 9600)
ser_receiver = serial.Serial(port_receiver, 4608000, timeout=10)
sleep(0.1)
ser_emitter.setRTS(False)
ser_receiver.setRTS(False)
sleep(0.1)
ser_receiver.setRTS(True)
ser_emitter.setRTS(True)


datas = []
for x in tqdm(range(42)):
    for i in range(42):
        ts_start = time.time()
        emitter_str = "z"
        if i == 0:
            emitter_str = "re"
        if x == 0 and i == 0:
            emitter_str += "t"
        emitter_str += "a"
        ser_receiver.write(
            str(
                "a"
                # chr(startMarker) + chr(len("hey")) + "hey" + chr(endMarker)
            ).encode()
        )
        # print("written to receiver!")

        # print("writing {} to emitter".format(emitter_str))
        # print("writing at ", time.time() - ts_start)
        ser_emitter.write(
            str(
                emitter_str
                # chr(startMarker) + chr(len("O")) + "O" + chr(endMarker)
            ).encode()
        )
        # for _ in range(len(emitter_str)):
        #     print("emitter sent", ser_emitter.readline())
        data = ser_receiver.read(BUF_SIZE * 2 * 2)
        if len(data) != BUF_SIZE * 2 * 2:
            print("one invalid frame of size", len(data))
            data2 = ser_receiver.read(BUF_SIZE * 2 * 2)
            print(len(data2), "is the length of the next stored message")
        data = struct.unpack("h" * (len(data) // 2), data)

        points = []
        for j in range(BUF_SIZE):
            points.append([data[2 * j], data[2 * j + 1]])
            """
        data1 = [e[0] for e in points]
        data2 = [e[1] for e in points]
        data1 = butter_bandpass_filter(data1, lowcut, highcut, fs, 2)
        data2 = butter_bandpass_filter(data2, lowcut, highcut, fs, 2)
        data1 = hilbert(np.abs(data1))
        data2 = hilbert(np.abs(data2))
        diff = data1 - data2
        plt.subplot(3, 1, 1)
        plt.plot(data1)
        plt.subplot(3, 1, 2)
        plt.plot(data2)
        plt.subplot(3, 1, 3)
        plt.title("angle: " + str(i))
        plt.plot(diff)
        plt.show()
        """
        if x > 21:
            data = [e[1] for e in points]
        else:
            data = [e[0] for e in points]

        datas.append(data)
        # print("total time", time.time() - ts_start)
# print(datas)
i = 0
while os.path.isfile("data{}.json".format(i)):
    i += 1

with open("data{}.json".format(i), "w") as dump:
    print("saving as data{}.json".format(i))
    dump.write(json.dumps(datas))
