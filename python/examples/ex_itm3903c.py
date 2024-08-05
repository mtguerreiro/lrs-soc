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

data = json.load(f)
datastr = str(data)
count = datastr.count("Board")

#--- Input ---
port = 8080

for board in data:
    
    if data[board]["status"] == "0":
        print(board + " not active")
        
    if data[board]["status"] == "1":

        print(board + " calibrated")
        
        host = data[board]["address"]
        settings = {'host':host, 'port':port}
        hw = lrssoc.itm3903c.itm3903c.ITM3903C(comm='ethernet', comm_settings=settings, cs_id=1)

        trace = lrssoc.itm3903c.itm3903c_trace.Trace(hw._ocp_if, 0)

        hw_if = hw._hw_if
        ahw_if = hw._an_hw_if
      
        ahw_if.set_dac_cal_data({   'dac1_adj_gain': data[board]["DACADJ"]["gain"],
                                    'dac1_adj_offset': data[board]["DACADJ"]["Offset"],
                                    'dac1_offset_gain': data[board]["DACOFFSET"]["gain"],
                                    'dac1_offset_offset':data[board]["DACOFFSET"]["Offset"],
                                    'dac2_gain': data[board]["DAC2"]["gain"],
                                    'dac2_offset': data[board]["DAC2"]["Offset"],
                                    'dac3_gain': data[board]["DAC3"]["gain"],
                                    'dac3_offset':data[board]["DAC3"]["Offset"]
                                })
        
        ahw_if.set_adc_cal_data({   'v_gain': data[board]["ADCV"]["gain"],
                                    'v_ofs': data[board]["ADCV"]["Offset"],
                                    'i_gain': data[board]["ADCC"]["gain"],
                                    'i_ofs':data[board]["ADCC"]["Offset"],
         
                                })
f.close()


def cal(x, y):

    a = np.polyfit(x, y, 1)

    return (a[0], a[1])