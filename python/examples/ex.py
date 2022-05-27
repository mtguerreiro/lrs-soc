import lrssoc

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
host = '131.246.75.210'
port = 8080

soc = lrssoc.hwi.Interface(host, port)

def plot_data():
    data = soc.cpu0_trace_read()
    n = int( len(data) / 2 )
    data = [[data[2*i], data[2*i+1]] for i in range(n)]
    data = lrssoc.conversions.u8_to_u16(data, msb=False)
    data = [~d & 0xFFF for d in data]
    #ch1 = np.array(data[::2])
    ch2 = np.array(data[1::2])

#    ch2_v = (ch2 /4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
    ch2_v = (ch2 /4095*5.14-2.571-0.01/2) * 22/10 / 150 *1000/2500 *4.989e3
    rms = np.sqrt(np.mean(ch2_v**2))
    print('RMS: {:.2f}\n'.format(rms))
    #plt.plot(ch1)
    plt.plot(ch2_v)

    return ch2
