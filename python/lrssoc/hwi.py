"""
Module ``hwi``
==============

Hardware interface for the LRS-SOC platform.

"""
import socket
import time
import struct
import lrssoc

class Commands:
    """Just a list with the commands accepted by the platform. They must match
    the commands that are defined in the `soc_defs.h` file from the hardware.
    """
    def __init__(self):
        self.cpu0_blink = 0
        self.cpu1_blink = 1
        self.cpu1_adc_en = 2
        self.cpu1_adc_spi_freq_set = 3
        self.cpu1_adc_sampling_freq_set = 4
        self.cpu1_adc_error_read = 5
        self.cpu1_adc_error_clear = 6
        self.cpu0_trace_start = 7
        self.cpu0_trace_read = 8
        self.cpu0_trace_size_set = 9
        self.cpu0_trace_size_read = 10
        self.cpu0_control_en = 11
        

class Interface:
    """A class to provide an interface to the LRS-SOC platform.

    Parameters
    ----------
    host : str
        IP address of the controller.

    port : int
        Port of the controller. By default, it is `8080`.

    Raises
    ------
    TypeError
        If `host` is not of `str` type.

    TypeError
        If `port` is not of `int` type.

    Attributes
    ----------
    host : str
        IP address of the controller.

    port : int
        Port of the controller. By default, it is `8080`.

    hwcomm : lrssoc.hwc.Ethernet
        Communication channel to the hardware.

    cmd : lrssoc.hwi.Commands
        Implemented hardware commands.
        
    """
    def __init__(self, host, port=8080):

        if type(host) is not str:
            raise TypeError('`host` must be of `str` type.')
        
        if type(port) is not int:
            raise TypeError('`port` must be of `int` type.')

        self.host = host
        self.port = port
        self.hwc = lrssoc.hwc.Ethernet(host, port)
        
        self.cmd = Commands()

            
    def cpu0_blink(self, t=1000):
        """Changes the blinking period of CPU0.

        Parameters
        ----------
        t : int
            Period, in milliseconds. By default, it is 1000.

        Raises
        ------
        TypeError
            If `t` is not of `int` type.

        """
        if type(t) is not int:
            raise TypeError('`t` must be of int type.')

        tx_data = []
        cmd = self.cmd.cpu0_blink
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(t, msb=False) )

        self.hwc.comm(tx_data)
        

    def cpu1_blink(self, t=1000):
        """Changes the blinking period of CPU1.

        Parameters
        ----------
        t : int
            Period, in milliseconds. By default, it is 1000.

        Raises
        ------
        TypeError
            If `t` is not of `int` type.

        """
        if type(t) is not int:
            raise TypeError('`t` must be of int type.')

        tx_data = []
        cmd = self.cmd.cpu1_blink
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(t, msb=False) )

        self.hwc.comm(tx_data)


    def cpu1_adc_en(self, en):
        """Enables/disables the ADC.

        Parameters
        ----------
        en : bool
            If `True`, enables ADC. If `False`, disables ADC.

        Raises
        ------
        TypeError
            If `en` is not of `bool` type.

        """        
        if type(en) is not bool:
            raise TypeError('`en` must be of bool type.')
        
        tx_data = []
        cmd = self.cmd.cpu1_adc_en

        if en == True:
            en = [1]
        else:
            en = [0]
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )

        self.hwc.comm(tx_data)

  
    def cpu1_adc_spi_freq_set(self, freq=10):
        """Changes ADC's SPI clock frequency.

        The clock will be divided by 2 x `freq`.

        Parameters
        ----------
        freq : int
            Clock division factor. By default, it is 10.

        Raises
        ------
        TypeError
            If `freq` is not of `int` type.

        """        
        tx_data = []
        cmd = self.cmd.cpu1_adc_spi_freq_set
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )

        self.hwc.comm(tx_data)


    def cpu1_adc_sampling_freq_set(self, freq=10000):
        """Changes ADC's sampling frequency.

        The frequency will be given by MAIN_CLOCK / (2 x `freq`).

        Parameters
        ----------
        freq : int
            Clock division factor. By default, it is 10000.

        Raises
        ------
        TypeError
            If `freq` is not of `int` type.

        """        
        tx_data = []
        cmd = self.cmd.cpu1_adc_sampling_freq_set
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )

        self.hwc.comm(tx_data)


    def cpu1_adc_error_read(self):
        """Reads the ADC's error flag.

        Returns
        ------
        int
            Error flag.

        """        
        tx_data = []
        cmd = self.cmd.cpu1_adc_error_read
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        data = self.hwc.comm(tx_data)

        error = lrssoc.conversions.u8_to_u32(data, msb=False)

        return error


    def cpu1_adc_error_clear(self):
        """Reads the ADC's error flag.

        Returns
        ------
        int
            Error flag.

        """        
        tx_data = []
        cmd = self.cmd.cpu1_adc_error_clear
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        self.hwc.comm(tx_data)


    def cpu0_trace_start(self):
        """Start recording data.

        """        
        tx_data = []
        cmd = self.cmd.cpu0_trace_start
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        self.hwc.comm(tx_data)


    def cpu0_trace_read(self, size=None):
        """Read recorded data.

        """        
        tx_data = []
        cmd = self.cmd.cpu0_trace_read
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        if size == None:
            size = 20000
            
        data = self.hwc.comm(tx_data, size)

        return data


    def cpu0_trace_size_set(self, size):
        """Sets the number of bytes to save.

        """
        tx_data = []
        cmd = self.cmd.cpu0_trace_size_set
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
        
        self.hwc.comm(tx_data)


    def cpu0_trace_size_read(self):
        """Gets the number of bytes saved

        """
        tx_data = []
        cmd = self.cmd.cpu0_trace_size_read
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        data = self.hwc.comm(tx_data)

        size = lrssoc.conversions.u8_to_u32(data, msb=False)

        return size


    def cpu1_control_en(self, en):
        """Enables/disables the control algorithm.

        Parameters
        ----------
        en : bool
            If `True`, enables control. If `False`, disables control.

        Raises
        ------
        TypeError
            If `en` is not of `bool` type.

        """        
        if type(en) is not bool:
            raise TypeError('`en` must be of bool type.')
        
        tx_data = []
        cmd = self.cmd.cpu0_control_en

        if en == True:
            en = [1]
        else:
            en = [0]
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )

        self.hwc.comm(tx_data)
