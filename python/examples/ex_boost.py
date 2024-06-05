import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()
from scipy.interpolate import interp1d

from numpy import asarray
from numpy import savetxt
from numpy import loadtxt
import time

import pickle

import scipy.signal

# --- Input ---
port = 8080

host = '131.246.75.145' #131.246.75.192
settings = {'host':host, 'port':port}

boost = lrssoc.boost.boost.Boost(0, 'ethernet', settings, 0)

plot = lrssoc.boost.boost_plot.Plot()

#status, (traces, data, t) = boost.read_trace()
#boost._hw_if.set_adc_spi_freq(10000000)
#boost._hw_if.set_pwm_inv()
#boost._hw_if.set_pwm_duty(0.5)

def save_data(file, data):

    with open(file + '.pkl', 'wb') as f:
        pickle.dump(data, f)

def load_data(file):

    with open(file + '.pkl', 'rb') as f:
        data = pickle.load(f)

    return data

