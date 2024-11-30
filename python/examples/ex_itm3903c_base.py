import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

import pickle

import scipy.signal

# --- Input ---
port = 8080
host = '192.168.0.102'

settings = {'host':host, 'port':port}

hw = lrssoc.itm3903c.itm3903c.ITM3903C(comm='ethernet', comm_settings=settings, cs_id=1)

trace = lrssoc.itm3903c.itm3903c_trace.Trace(hw._ocp_if, 0)

hw_if = hw._hw_if
ahw_if = hw._an_hw_if

#status, (traces, data) = trace.read()

def cal(x, y):

    a = np.polyfit(x, y, 1)

    return (a[0], a[1])
