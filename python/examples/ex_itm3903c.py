import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

import json

import pickle

import scipy.signal

f = open('calibrationinformation2.json')

file = json.load(f)

hw_dict = {}
ahw_dict = {}
data_dict = {}
trace = {}

for board in file:
    
    host = file[board]["address"]
    print(host)
    port = 8080
    settings = {'host':host, 'port':port}
    hw_dict[board] = lrssoc.itm3903c.itm3903c.ITM3903C(comm='ethernet', comm_settings=settings, cs_id=1)
    print(hw_dict[board])
    trace[board] = lrssoc.itm3903c.itm3903c_trace.Trace(hw_dict[board]._ocp_if, 0)
    print(trace[board])

    hw_if = hw_dict[board]._hw_if
    ahw_if = hw_dict[board]._an_hw_if

    hw_dict["Board 1"]._an_hw_if.set_dac1_offset(5)
       
   
def graphOffset():

    fig, axs = plt.subplots(2)

    t_min = max(data_dict["Board 1"][2][0],data_dict["Board 2"][2][0])
    t_max = min(data_dict["Board 1"][2][1999],data_dict["Board 2"][2][1999])
    
    x_min = t_min /1000000
    x_max = t_max /1000000
    #x_min = 0
    #x_max = 0
    print(x_min, x_max)
    
    axs[0].set_xlim(0, x_max - x_min)
    axs[1].set_xlim(0, x_max - x_min)
    
    axs[0].set_ylabel("Voltage (V)")
    axs[0].set_xlabel("Time (s)")
    axs[0].plot(((np.array(data_dict["Board 1"][2])/1000000) - x_min), np.array((data_dict["Board 1"][0])) * 80*(14/13), color = 'red', label = "Source")
    axs[0].plot(((np.array(data_dict["Board 2"][2])/1000000) - x_min), np.array((data_dict["Board 2"][0])) * 80*(14/13), color = 'black', label= "Load")
    axs[0].title.set_text('Output Voltages')
        
    axs[1].set_ylabel("Current (A)")
    axs[1].set_xlabel("Time (s)")
    axs[1].plot(((np.array(data_dict["Board 1"][2])/1000000) - x_min), np.array((data_dict["Board 1"][1])) * (16/20)*(14/13), color = 'red', label = "Source")
    axs[1].plot(((np.array(data_dict["Board 2"][2])/1000000) - x_min), np.array((data_dict["Board 2"][1])) * (16/20)*(14/13), color = 'black', label = "Load")
    axs[1].title.set_text('Output Currents')
    
    axs[1].legend()
    axs[0].legend()

def setSample():
    for board in trace:

        hw_dict[board]._an_hw_if.set_sampling_status(True)
        hw_dict[board]._an_hw_if.set_sampling_freq(1000)
        trace[board].set_size(2000)

def prepRead():

    trace["Board 1"].reset()
    trace["Board 2"].reset()

    hw_dict["Board 1"]._an_hw_if.set_dac1_offset(5)
    hw_dict["Board 2"]._an_hw_if.set_dac1_offset(0)
    
    time.sleep(0.2)
    
    hw_dict["Board 2"]._an_hw_if.set_dac1_offset(5)

    time.sleep(0.2)
    
    hw_dict["Board 1"]._an_hw_if.set_dac1_offset(7)

    time.sleep(0.2)

    hw_dict["Board 1"]._an_hw_if.set_dac1_offset(5)
    hw_dict["Board 2"]._an_hw_if.set_dac1_offset(0)

    time.sleep(2)
    
    status, (traces, data_dict["Board 2"]) = trace["Board 2"].read()
    status, (traces, data_dict["Board 1"]) = trace["Board 1"].read()

def setAnalogLimits(board):
    hw_dict[board]._an_hw_if.set_dac23_a2(10)
    hw_dict[board]._an_hw_if.set_dac23_a3(0)
