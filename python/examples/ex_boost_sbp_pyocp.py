import pyocp
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

import pickle

import scipy.signal

# --- Input ---
port = 8080
#port = 8085

host = '131.246.75.131'
#host = 'localhost'
settings = {'host':host, 'port':port}

boost = pyocp.lrs.boost_sbp.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)

