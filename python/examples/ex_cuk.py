import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

# --- Input ---
host = '127.0.0.1'
port = 8080
settings = {'host':host, 'port':port}

#soc = lrssoc.hwi.Interface(comm_type='ethernet', settings=settings)

cuk = lrssoc.cuk.cuk.Cuk(0, 'ethernet', settings, 0)

#cuk.trace_set_size(4 * 100000)
cuk.trace_set_size( round(200e-3 / (1 / 100e3)) )
cuk.trace_reset()

#cuk.control_sys_enable()
#cuk.controller_enable('ol')

#cuk.control_sys_enable()
#cuk.controller_set_params('ol', {'u':0.5})
#cuk.controller_enable('ol', reset=False)
#cuk.control_sys_disable()

#status, (traces, data) = cuk.trace_read(); data = np.array(data).T

#cuk.controller_set_params('ol', {'u':0.25}); cuk.trace_reset(); time.sleep(0.1); cuk.controller_set_params('ol', {'u':0.5}); time.sleep(0.1); cuk.controller_set_params('ol', {'u':0.25}); time.sleep(0.4); status, (traces, data) = cuk.trace_read(); data = np.array(data).T; cuk.controller_set_params('ol', {'u':0.0});

def live():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 0])
        ax.plot(data[:, 1])
        ax.plot(data[:, 2])
        ax.plot(data[:, 3])
        ax.plot(data[:, 4])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)

def live_voltages():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 2])
        ax.plot(data[:, 3])
        ax.plot(data[:, 4])
        ax.plot(data[:, 7])
        ax.plot(data[:, 8])
        ax.plot(data[:, 9])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)


def live_voltages_prim():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 2])
        ax.plot(data[:, 3])
        ax.plot(data[:, 4])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)


def live_currents_prim():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 0])
        ax.plot(data[:, 1])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)
        

def live_voltages_sec():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 7])
        ax.plot(data[:, 8])
        ax.plot(data[:, 9])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)


def live_currents_sec():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 5])
        ax.plot(data[:, 6])
        
        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)


def lin_coef(y1, y2, x1, x2):

    a = (y2 - y1) / (x2 - x1)
    b = y1 - x1 * a

    return (a, b)


def run():
    

    for i in range(11):
        cuk.controller_set_params('ol', {'u':i*0.05})
        time.sleep(0.1)

    cuk._hw_if.set_output_switch(0)
    cuk.trace_set_size(1.5 * 100000)
    cuk.trace_reset()
    time.sleep(0.5)

    cuk._hw_if.set_output_switch(1)
    time.sleep(0.5)
    
    cuk._hw_if.set_output_switch(0)
    time.sleep(1.0)

    status, (traces, data) = cuk.trace_read(); data = np.array(data).T


    for i in range(11):
        cuk.controller_set_params('ol', {'u':0.5 - i*0.05})
        time.sleep(0.1)


    return data
