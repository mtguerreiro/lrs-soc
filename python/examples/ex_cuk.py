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

host_sim = '127.0.0.1'
settings_sim = {'host':host_sim, 'port':port}

host_hw = '131.246.75.169'
settings_hw = {'host':host_hw, 'port':port}

cuk_hw = lrssoc.cuk.cuk.Cuk(0, 'ethernet', settings_hw, 0)
cuk_sim = lrssoc.cuk.cuk.Cuk(0, 'ethernet', settings_sim, 0)

plot = lrssoc.cuk.cuk_plot.Plot()

#status, (traces, data, t) = cuk.read_trace()

def lin_coef(y1, y2, x1, x2):

    a = (y2 - y1) / (x2 - x1)
    b = y1 - x1 * a

    return (a, b)


def ramp(cuk, k=1.0):

    cuk.set_load_switch(0)
    cuk.startup_ctl_set_params(uinc=0.0005, ufinal=0.0)
    cuk.startup_ctl_enable(reset=False)
    cuk.set_trace_size(round(0.1 * 100000))

    cuk.reset_trace()
    time.sleep(0.02 * k);

    cuk.startup_ctl_set_params(uinc=0.0005, ufinal=0.47)
    time.sleep(0.02 * k);

    cuk.set_load_switch(1)
    time.sleep(0.01 * k);

    cuk.set_load_switch(0)
    time.sleep(0.01 * k);
    
    cuk.startup_ctl_set_params(uinc=-0.0005)

    time.sleep(0.15 * k);

    status, (traces, data, t) = cuk.read_trace()

    return (data, t)


def energy(cuk, k=1.0):

    cuk.set_load_switch(0)
    cuk.startup_ctl_set_params(uinc=0.0005, ufinal=0.0)
    cuk.startup_ctl_enable(reset=False)
    cuk.set_trace_size(round(0.15 * 100000))

    cuk.reset_trace()
    time.sleep(0.02 * k);

    cuk.startup_ctl_set_params(uinc=0.0005, ufinal=0.47)
    time.sleep(0.02 * k);

    cuk.energy_ctl_enable(reset=False)
    time.sleep(0.01 * k);

    #cuk.set_load_switch(1)
    cuk.set_ref(32.0)
    time.sleep(0.01 * k);

    #cuk.set_load_switch(0)
    cuk.set_ref(30.0)
    time.sleep(0.01 * k);
    
    cuk.startup_ctl_enable(reset=False)
    time.sleep(0.01 * k);
    
    cuk.startup_ctl_set_params(uinc=-0.0005, ufinal=0.47)

    time.sleep(0.15 * k);

    status, (traces, data, t) = cuk.read_trace()

    return (data, t)


def save_data(file, data):

    with open(file + '.pkl', 'wb') as f:
        pickle.dump(data, f)

def load_data(file):

    with open(file + '.pkl', 'rb') as f:
        data = pickle.load(f)

    return data

