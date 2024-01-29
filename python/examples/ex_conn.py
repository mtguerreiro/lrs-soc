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
hw = lrssoc.ocp.iface.Interface(settings=settings)

trace = lrssoc.cuk.cuk_trace.Trace(hw, 0)

#status, (traces, data) = trace.read()
