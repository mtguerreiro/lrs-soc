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

host = '131.246.75.169'
settings = {'host':host, 'port':port}

boost = lrssoc.boost.boost.Boost(0, 'ethernet', settings, 0)

plot = lrssoc.boost.boost_plot.Plot()

#status, (traces, data, t) = boost.read_trace()


def save_data(file, data):

    with open(file + '.pkl', 'wb') as f:
        pickle.dump(data, f)

def load_data(file):

    with open(file + '.pkl', 'rb') as f:
        data = pickle.load(f)

    return data

