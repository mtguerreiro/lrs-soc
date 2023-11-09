import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

# --- Input ---
port = 8080

host_sim = '127.0.0.1'
settings_sim = {'host':host_sim, 'port':port}

host_hw = '131.246.75.169'
settings_hw = {'host':host_hw, 'port':port}

cuk_hw = lrssoc.cuk.cuk.Cuk(0, 'ethernet', settings_hw, 0)
cuk_sim = lrssoc.cuk.cuk.Cuk(0, 'ethernet', settings_sim, 0)

plot = lrssoc.cuk.cuk_plot.Plot()

#cuk.trace_set_size(4 * 100000)
#cuk.trace_set_size( round(200e-3 / (1 / 100e3)) )
#cuk.trace_reset()

#cuk.control_sys_enable()
#cuk.controller_enable('ol')

#cuk.control_sys_enable()
#cuk.controller_set_params('ol', {'u':0.5})
#cuk.controller_enable('ol', reset=False)
#cuk.control_sys_disable()

#status, (traces, data, t) = cuk.trace_read()


def lin_coef(y1, y2, x1, x2):

    a = (y2 - y1) / (x2 - x1)
    b = y1 - x1 * a

    return (a, b)


def run():

    cuk._hw_if.set_load_switch(0)

    cuk.controller_set_params('startup', {'uinc': 0.0005000000237487257, 'ufinal': 0.0})
    cuk.controller_enable('startup', reset=False)
    cuk.trace_set_size(0.2 * 100000)
    cuk.trace_reset()
    time.sleep(0.02);
    
    cuk.controller_set_params('startup', {'uinc': 0.0005000000237487257, 'ufinal': 0.5})
    time.sleep(0.02)

    cuk._hw_if.set_load_switch(1)
    time.sleep(0.02)
    
    cuk._hw_if.set_load_switch(0)
    time.sleep(0.02)

    cuk.controller_set_params('startup', {'uinc': -0.0005000000237487257})

    time.sleep(0.02)

    status, (traces, data, t) = cuk.trace_read()

    return t, data


def ramp(cuk, k=1.0):

    cuk._hw_if.set_load_switch(0)
    cuk.controller_set_params('startup', {'uinc': 0.0005000000237487257, 'ufinal': 0.0})
    cuk.controller_enable('startup', reset=False)
    cuk.trace_set_size(round(0.1 * 100000))

    cuk.trace_reset()
    time.sleep(0.02 * k);

    cuk.controller_set_params('startup', {'uinc': 0.0005000000237487257, 'ufinal': 0.47})
    time.sleep(0.02 * k);

    cuk._hw_if.set_load_switch(1)
    time.sleep(0.01 * k);

    cuk._hw_if.set_load_switch(0)
    time.sleep(0.01 * k);
    
    cuk.controller_set_params('startup', {'uinc': -0.0005000000237487257, 'ufinal': 0.47})

    time.sleep(0.2 * k);

    status, (traces, data, t) = cuk.trace_read()

    return (data, t)
