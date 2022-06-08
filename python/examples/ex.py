import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
host = '131.246.75.210'
port = 8080

soc = lrssoc.hwi.Interface(host, port)

soc.cpu1_adc_sampling_freq_set(20000)
soc.cpu1_adc_spi_freq_set(40)
#soc.cpu0_trace_size_set(10000)
#soc.cpu0_trace_size_set(250000)
soc.cpu0_trace_size_set(500000)

def read_adc_data():
    trace_size = soc.cpu0_trace_size_read()    
    data = soc.cpu0_trace_read(trace_size)
    n = int( len(data) / 2 )
    data = [[data[2*i], data[2*i+1]] for i in range(n)]
    data = lrssoc.conversions.u8_to_u16(data, msb=False)

    ch1 = np.array(data[::8])
    ch2 = np.array(data[2::8])
    ch3 = np.array(data[4::8])
    ch4 = np.array(data[6::8])

    return ch1, ch2, ch3, ch4


def read_adc_data2():
    trace_size = soc.cpu0_trace_size_read()
    data = soc.cpu0_trace_read(trace_size)
    n = int( len(data) / 4 )
    data = [data[4*i:(4*i+4)][::-1] for i in range(n)]
    data = [struct.unpack('!f', d) for d in data]

    ch1 = np.array(data[::12])
    ch2 = np.array(data[1::12])
    ch3 = np.array(data[2::12])
    ch4 = np.array(data[3::12])
    ch5 = np.array(data[4::12])
    ch6 = np.array(data[5::12])
    ch7 = np.array(data[6::12])
    ch8 = np.array(data[7::12])
    ch9 = np.array(data[8::12])
    ch10 = np.array(data[9::12])
    ch11 = np.array(data[10::12])
    ch12 = np.array(data[11::12])
        
    return ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10, ch11, ch12


def plot_adc_data():

    data = read_adc_data()

    ch1 = (data[0] / 4095 * 5 - 2.5) / 185e-3
    ch2 = data[1] / 4095 * 5 * (900 / 23.2)
    ch3 = (data[2]/4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
    ch4 = (data[3]/4095*5-2.5) * 22/10 / 150 * 1000

    plt.figure(figsize=(10,7))
    ax = plt.subplot(2,2,2)
    plt.title('HB current')
    plt.plot(ch1)

    plt.subplot(2,2,3, sharex=ax)
    plt.title('DC link voltage')
    plt.plot(ch2)

    plt.subplot(2,2,1, sharex=ax)
    plt.title('Grid voltage')
    plt.plot(ch3)

    plt.subplot(2,2,4, sharex=ax)
    plt.title('Load current')
    plt.plot(ch4)

    plt.tight_layout()


def plot_adc_data2(data=None):

    if data is None:
        data = read_adc_data2()

    ch1 = data[0]
    ch2 = data[1]
    ch3 = data[2]
    ch4 = data[3]
    ch5 = data[4]
    ch6 = data[5]
    ch7 = data[6]
    ch8 = data[7]
    ch9 = data[8]
    ch10 = data[9]
    ch11 = data[10]
    ch12 = data[11]
    
    fig, ((ax1, ax2, ax3, ax4), (ax5, ax6, ax7, ax8), (ax9, ax10, ax11, ax12)) = plt.subplots(3,4, figsize=(10,7), sharex=True)
    
    ax1.cla()
    ax1.set_title('Grid voltage')
    ax1.plot(ch3)

    ax2.cla()
    ax2.set_title('HB current')
    ax2.plot(ch1)        

    ax3.cla()
    ax3.set_title('DC link')
    ax3.plot(ch2)
    
    ax4.cla()
    ax4.set_title('Load current')
    ax4.plot(ch4)

    ax5.cla()
    ax5.set_title('Duty')
    ax5.plot(ch5)

    ax6.cla()
    ax6.set_title('$u_{pi}$')
    ax6.plot(ch6)
    
    ax7.cla()
    ax7.set_title('$u_{pr}$')
    ax7.plot(ch7)
    
    ax8.cla()
    ax8.set_title('Nada')
    ax8.plot(ch8)

    ax9.cla()
    ax9.set_title('DC link inst')
    ax9.plot(ch9)

    ax10.cla()
    ax10.set_title('$e_{pi}$')
    ax10.plot(ch10)
    
    ax11.cla()
    ax11.set_title('$e_{pr}$')
    ax11.plot(ch11)
    
    ax12.cla()
    ax12.set_title('$i_{g,ref}$')
    ax12.plot(ch12)
        
    plt.tight_layout()

    
def plot_data():
    data = soc.cpu0_trace_read()
    n = int( len(data) / 2 )
    data = [[data[2*i], data[2*i+1]] for i in range(n)]
    data = lrssoc.conversions.u8_to_u16(data, msb=False)
    #data = [~d & 0xFFF for d in data]
    ch1 = np.array(data[::2])
    #ch2 = np.array(data[1::2])

    #ch2_v = (ch2 /4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
    #ch2_v = (ch2 /4095*5.14-2.571-0.01/2) * 22/10 / 150 *1000/2500 *4.989e3
    #rms = np.sqrt(np.mean(ch2_v**2))
    #print('RMS: {:.2f}\n'.format(rms))
    #plt.plot(ch1)
    print(ch1[0])
    plt.plot(ch1)

#    return ch1


##def plot_data():
##    data = soc.cpu0_trace_read()
##    n = int( len(data) / 2 )
##    data = [[data[2*i], data[2*i+1]] for i in range(n)]
##    data = lrssoc.conversions.u8_to_u16(data, msb=False)
##    data = [~d & 0xFFF for d in data]
##    #ch1 = np.array(data[::2])
##    ch2 = np.array(data[1::2])
##
###    ch2_v = (ch2 /4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
##    ch2_v = (ch2 /4095*5.14-2.571-0.01/2) * 22/10 / 150 *1000/2500 *4.989e3
##    rms = np.sqrt(np.mean(ch2_v**2))
##    print('RMS: {:.2f}\n'.format(rms))
##    #plt.plot(ch1)
##    plt.plot(ch2_v)
##
##    return ch2

def plot_data_cont():

    trace_size = soc.cpu0_trace_size_read()
    soc.cpu0_trace_start()
    soc.cpu1_adc_en(True)

    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2,2, figsize=(10,7))

    while True:
        data = soc.cpu0_trace_read(trace_size)
        n = int( len(data) / 2 )
        data = [[data[2*i], data[2*i+1]] for i in range(n)]
        data = lrssoc.conversions.u8_to_u16(data, msb=False)

        ch1 = np.array(data[::8])
        ch2 = np.array(data[2::8])
        ch3 = np.array(data[4::8])
        ch4 = np.array(data[6::8])

        ch1 = (ch1 / 4095 * 5 - 2.5) / 185e-3
        ch2 = ch2 / 4095 * 5 * (900 / 23.2)
        ch3 = (ch3/4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
        ch4 = (ch4/4095*5-2.5) * 22/10 / 150 * 1000

        ax1.cla()
        ax1.set_title('Grid voltage')
        ax1.plot(ch3)

        ax2.cla()
        ax2.set_title('HB current')
        ax2.plot(ch1)        

        ax3.cla()
        ax3.set_title('DC link voltage')
        ax3.plot(ch2)
        
        ax4.cla()
        ax4.set_title('Load current')
        ax4.plot(ch4)

        plt.tight_layout()

        soc.cpu0_trace_start()
        plt.pause(trace_size / 2 / 8 * 1 / 5e3)
        
##def plot_data_cont():
##
##    soc.cpu0_trace_start()
##    soc.cpu1_adc_en(True)
##
##    plt.figure(figsize=(10,7))
##    
##    while True:
##        data = soc.cpu0_trace_read()
##        n = int( len(data) / 2 )
##        data = [[data[2*i], data[2*i+1]] for i in range(n)]
##        data = lrssoc.conversions.u8_to_u16(data, msb=False)
##
##        ch1 = np.array(data[::8])
##        ch2 = np.array(data[2::8])
##        ch3 = np.array(data[4::8])
##        ch4 = np.array(data[6::8])
##
##        ch1 = (ch1 / 4095 * 5 - 2.5) / 185e-3
##        ch2 = ch2 / 4095 * 5 * (900 / 23.2)
##        ch3 = (ch3/4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
##        ch4 = (ch4/4095*5-2.5) * 22/10 / 150 * 1000
##        
##        plt.subplot(2,2,2)
##        plt.gca().cla()
##        plt.title('HB current')
##        plt.plot(ch1)
##
##        plt.subplot(2,2,3)
##        plt.gca().cla()
##        plt.title('DC link voltage')
##        plt.plot(ch2)
##
##        plt.subplot(2,2,1)
##        plt.gca().cla()
##        plt.title('Grid voltage')
##        plt.plot(ch3)
##
##        plt.subplot(2,2,4)
##        plt.gca().cla()
##        plt.title('Load current')
##        plt.plot(ch4)
##
##        plt.tight_layout()
##        plt.pause(0.5)


def plot_data_cont2():

    trace_size = soc.cpu0_trace_size_read()
    soc.cpu0_trace_start()
    soc.cpu1_adc_en(True)

    fig, ((ax1, ax2, ax3, ax4), (ax5, ax6, ax7, ax8), (ax9, ax10, ax11, ax12)) = plt.subplots(3,4, figsize=(10,7), sharex=True)

    while True:
        data = soc.cpu0_trace_read(trace_size)
        n = int( len(data) / 4 )
        data = [data[4*i:(4*i+4)][::-1] for i in range(n)]
        data = [struct.unpack('!f', d) for d in data]

        ch1 = np.array(data[::12])
        ch2 = np.array(data[1::12])
        ch3 = np.array(data[2::12])
        ch4 = np.array(data[3::12])
        ch5 = np.array(data[4::12])
        ch6 = np.array(data[5::12])
        ch7 = np.array(data[6::12])
        ch8 = np.array(data[7::12])
        ch9 = np.array(data[8::12])
        ch10 = np.array(data[9::12])
        ch11 = np.array(data[10::12])
        ch12 = np.array(data[11::12])

        #ch1 = (ch1 / 4095 * 5 - 2.5) / 185e-3
        #ch2 = ch2 / 4095 * 5 * (900 / 23.2)
        #ch3 = (ch3/4095*5-2.5) * 22/10 / 150 *1000/2500 *5e3
        #ch4 = (ch4/4095*5-2.5) * 22/10 / 150 * 1000

        ax1.cla()
        ax1.set_title('Grid voltage')
        ax1.plot(ch3)

        ax2.cla()
        ax2.set_title('HB current')
        ax2.plot(ch1)        

        ax3.cla()
        ax3.set_title('DC link voltage')
        ax3.plot(ch2)
        
        ax4.cla()
        ax4.set_title('Load current')
        ax4.plot(ch4)

        ax5.cla()
        ax5.set_title('Duty')
        ax5.plot(ch5)

        ax6.cla()
        ax6.set_title('$u_{pi}$')
        ax6.plot(ch6)
        
        ax7.cla()
        ax7.set_title('$u_{pr}$')
        ax7.plot(ch7)
        
        ax8.cla()
        ax8.set_title('DC link inst')
        ax8.plot(ch8)

        ax9.cla()
        ax9.set_title('Duty')
        ax9.plot(ch9)

        ax10.cla()
        ax10.set_title('$u_{pi}$')
        ax10.plot(ch10)
        
        ax11.cla()
        ax11.set_title('$u_{pr}$')
        ax11.plot(ch11)
        
        ax12.cla()
        ax12.set_title('DC link inst')
        ax12.plot(ch12)
        
        plt.tight_layout()

        soc.cpu0_trace_start()
        plt.pause(trace_size / (12*4) * 1 / 5e3)
