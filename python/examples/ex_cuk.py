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

cuk.trace_set_size(4 * 100000)
cuk.trace_reset()

#status, (traces, data) = cuk.trace_read(); data = np.array(data).T

def live():

    cuk.trace_set_size(0.5 * 100000)

    fig = plt.figure()
    ax = plt.gca()
    while True:
        status, (traces, data) = cuk.trace_read()
        data = np.array(data).T

        ax.cla()
        ax.plot(data[:, 4])

        plt.tight_layout()

        cuk.trace_reset()
        plt.pause(0.5)

        
