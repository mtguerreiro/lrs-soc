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

host = '131.246.75.251'
#host = 'localhost'
settings = {'host':host, 'port':port}

bb = pyocp.lrs.fsbuckboost.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)


def shutdown():

    bb.ramp.set_params({'u_step':0.001, 'u_ref':0.0})
    bb.ramp.enable()
    time.sleep(0.5)
    bb.disable()
    
def init():
    bb.hw.clear_status()
    bb.set_converter_mode('buck')
    bb.set_ref(1)
    bb.hw.set_pwm_dead_time(150)
    bb.idle.enable()
    bb.enable()
    bb.disable()
    bb.hw.clear_status()
    bb.idle.enable()
    bb.enable()
    bb.hw.set_input_relay(1)
    bb.hw.set_output_relay(1)


def cfg_trace():
    bb.trace.set_mode(1)
    bb.trace.set_size(10000)
    bb.trace.set_n_pre_trig_samples(5000)
    bb.trace.set_trig_level(7)
    bb.trace.set_trig_signal(8)

def cfg_sfb(ts, os):

    bb.buck_sfb.set_gains(ts, os, dt=1/100e3)
    bb.buck_sfb.reset()

    
def run():

    #ts = [0.5e-3, 1e-3, 2e-3]
    ts = [5e-3]
    os = 5

    cfg_trace()

    data_set = []
    for t in ts:
        init()
        cfg_sfb(t, 5)

        bb.trace.reset()

        bb.set_ref(5)
        bb.buck_sfb.enable()
        bb.trace.reset()
        time.sleep(0.5)

        bb.set_ref(10)
        time.sleep(0.5)

        bb.set_ref(5)
        time.sleep(0.5)
        
        shutdown()

        status, data = bb.trace.read()
        data_set.append(data)

    return data_set


def plot(data_set, signal):

    plt.figure()
    plt.grid()
    for data in data_set:
        plt.plot(data[:, signal])

    plt.tight_layout()
