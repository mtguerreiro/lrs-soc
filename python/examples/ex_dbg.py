import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
host = '131.246.75.192'
port = 8080
settings = {'host':host, 'port':port}

soc = lrssoc.hwi.Interface(comm_type='ethernet', settings=settings)

afe = lrssoc.afe.AFE(comm_type='ethernet', settings=settings)


def config():
    afe.adc_config(40)
    afe.pwm_config(20000)
    afe.set_controller(0)
    afe.hwi.cs_enable(0)
    afe.hwi.trace_set_size(0, 4096)

def start_acq():
    afe.adc_en(False)
    afe.hwi.trace_reset(0)
    afe.adc_en(True)

def stop_acq():
    afe.adc_en(False)

def read():
    status, data = afe.hwi.trace_read(0)
    data = np.array(data)
    n = int(data.shape[0] / 4)
    data = data.reshape(n, 4)
    return data
