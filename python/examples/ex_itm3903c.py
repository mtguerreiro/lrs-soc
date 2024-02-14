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
host = '131.246.75.132'

settings = {'host':host, 'port':port}

hw = lrssoc.itm3903c.itm3903c.ITM3903C(0, comm='ethernet', comm_settings=settings)

trace = lrssoc.itm3903c.itm3903c_trace.Trace(hw._ocp_if, 0)

hw_if = hw._hw_if

#status, (traces, data) = trace.read()
