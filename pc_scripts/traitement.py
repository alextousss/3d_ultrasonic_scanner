import json
from joblib import Parallel, delayed
import multiprocessing
from tqdm import tqdm
import sys
from time import sleep
import math
import serial
import serial.tools.list_ports
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, lfilter, hilbert
from mpl_toolkits.mplot3d import Axes3D


num_cores = multiprocessing.cpu_count()


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


def traitement(arr):
    arr = butter_bandpass_filter(arr, lowcut, highcut, fs, 2)
    for i in range(len(arr)):
        arr[i] = arr[i] * t(i)
    arr = np.abs(arr)
    arr = arr + 1
    arr = np.log10(arr)
    return arr


fs = 138888
lowcut = 38000
highcut = 42000


datas = []
with open(sys.argv[1]) as dump:
    datas = json.loads(dump.read())
plt.plot(datas[int(len(datas) // 1.5)])
plt.show()
# datas = Parallel(n_jobs=16)(delayed(traitement)(data) for data in datas)

for x in tqdm(range(len(datas))):

    ##    plt.subplot(2, 1, 1)
    # plt.subplot(2, 5, 1)
    # plt.plot(datas[x])
    # plt.show()
    # for i in range(9):
    # plt.subplot(2, 5, i + 2)
    datas[x] = butter_bandpass_filter(datas[x], lowcut, highcut, fs, 2)
    #   plt.subplot(2, 1, 2)
    #   plt.plot(datas[x])
    #   plt.show()
    # for j in range(3000):
    #    data[j] = data[10000]
    # plt.plot(data)
    # plt.title(str(i) + "th order")
    # plt.show()

    # for i in range(3000):
    #    datas[x][i] = datas[x][10000]
    # plt.subplot(2, 2, 2)
    # plt.plot(datas[x])
    for i in range(len(datas[x])):
        datas[x][i] = datas[x][i] * t(i)
    # plt.subplot(2, 2, 3)
    # plt.plot(datas[x])
    #  datas[x] = hilbert(datas[x])
    datas[x] = np.abs(datas[x])
    datas[x] = datas[x] + 1
    # plt.subplot(2, 2, 4)
    # plt.plot(datas[x])
    # plt.show()

    datas[x] = np.log10(datas[x])
# datas[x][i] = math.log(1 + datas[x][i])


Xs = []
Ys = []
Zs = []
Cs = []
# plt.plot(datas[0])
# plt.show()
max_r = float(sys.argv[3])
min_c = float(sys.argv[2])
for x in tqdm(range(len(datas))):
    theta = (math.pi / 2) + ((x // 42) - 21) / 42 * (40 / 360) * 2 * math.pi
    phi = -((x % 42) - 21) / 42 * (40 / 360) * 2 * math.pi
    last_r = 0
    for i in range(len(datas[x])):
        r = t(i)
        if (
            datas[x][i] > min_c and r - last_r > 0.01 and r < max_r
        ):  # un tous les 5 centimètres
            #            if r > 20:
            #                print(r)
            last_r = r
            Xs.append(r * math.cos(phi) * math.sin(theta))
            Ys.append(r * math.sin(phi) * math.sin(theta))
            Zs.append(r * math.cos(theta))
            Cs.append(datas[x][i])
print("saved", len(Xs), "points")
#            break
"""
ax = plt.axes(projection="3d")
limit = 5
ax.set_xlim([0, limit])
ax.set_ylim([0, limit])
ax.set_zlim([-limit / 2, limit / 2])
ax.scatter3D([0], [0], [0], cmap="Reds")
ax.scatter3D(Xs, Ys, Zs, c=Cs, cmap="Reds")
plt.show()
"""
with open("pointcloud.csv", "w") as pcfile:
    for i in range(len(Xs)):
        pcfile.write(
            str(Xs[i])
            + ", "
            + str(Ys[i])
            + ", "
            + str(Zs[i])
            + ", "
            + str(Cs[i])
            + "\n"
        )
