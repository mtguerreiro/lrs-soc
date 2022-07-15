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

        cmd = self.cmd.cpu0_blink

        tx_data = []        
        tx_data.extend( lrssoc.conversions.u32_to_u8(t, msb=False) )

        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu0_blink.__name__
            print('{:}: Error blink CPU0 LED. Error code {:}\r\n'.format(funcname, status))

        return status
        

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

        cmd = self.cmd.cpu1_blink

        tx_data = []        
        tx_data.extend( lrssoc.conversions.u32_to_u8(t, msb=False) )

        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu1_blink.__name__
            print('{:}: Error blink CPU1 LED. Error code {:}\r\n'.format(funcname, status))
            
        return status
    

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
        
        cmd = self.cmd.cpu1_adc_en

        if en == True:
            en = [1]
        else:
            en = [0]

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )

        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu1_adc_en.__name__
            print('{:}: Error enabling ADC. Error code {:}\r\n'.format(funcname, status))
            
        return status

    
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
        cmd = self.cmd.cpu1_adc_spi_freq_set

        tx_data = []        
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )

        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu1_adc_spi_freq_set.__name__
            print('{:}: Error setting ADC SPI frequency. Error code {:}\r\n'.format(funcname, status))
            
        return status
    

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
        cmd = self.cmd.cpu1_adc_sampling_freq_set

        tx_data = []        
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )

        status, data = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu1_adc_spi_freq_set.__name__
            print('{:}: Error setting ADC sampling frequency. Error code {:}\r\n'.format(funcname, status))

        return status, data


    def cpu1_adc_error_read(self):
        """Reads the ADC's error flag.

        Returns
        ------
        int
            Error flag.

        """        
        cmd = self.cmd.cpu1_adc_error_read
        status, data = self.hwc.comm(cmd)

        if status != 0:
            funcname = Interface.cpu1_adc_error_read.__name__
            print('{:}: Error reading ADC error flag. Error code {:}\r\n'.format(cpu1_adc_error_read, status))
            return (-1, status)

        error = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, error)


    def cpu1_adc_error_clear(self):
        """Reads the ADC's error flag.

        Returns
        ------
        int
            Error flag.

        """        
        cmd = self.cmd.cpu1_adc_error_clear
        
        status, _ = self.hwc.comm(cmd)

        if status != 0:
            funcname = Interface.cpu1_adc_error_clear.__name__   
            print('{:}: Error clearing ADC error flag. Error code {:}\r\n'.format(funcname, status))

        return status
    

    def cpu0_trace_start(self):
        """Start recording data.

        """        
        cmd = self.cmd.cpu0_trace_start

        status, _ = self.hwc.comm(cmd)

        if status != 0:
            funcname = Interface.cpu0_trace_start.__name__
            print('{:}: Error starting trace. Error code {:}\r\n'.format(funcname, status))

        return status
    

    def cpu0_trace_read(self):
        """Read recorded data.

        """
        cmd = self.cmd.cpu0_trace_read
            
        status, data = self.hwc.comm(cmd)

        if status != 0:
            funcname = Interface.cpu0_trace_read.__name__
            print('{:}: Error reading trace. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0, data)


    def cpu0_trace_size_set(self, size):
        """Sets the number of bytes to save.

        """
        cmd = self.cmd.cpu0_trace_size_set

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
        
        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu0_trace_size_set.__name__
            print('{:}: Error setting trace size. Error code {:}\r\n'.format(funcname, status))

        return status
    

    def cpu0_trace_size_read(self):
        """Gets the number of bytes saved

        """
        cmd = self.cmd.cpu0_trace_size_read
                
        status, data = self.hwc.comm(cmd)

        if status != 0:
            funcname = Interface.cpu0_trace_size_read.__name__
            print('{:}: Error reading trace size. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        size = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, size)


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
        
        cmd = self.cmd.cpu0_control_en

        if en == True:
            en = [1]
        else:
            en = [0]

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )

        status, _ = self.hwc.comm(cmd, tx_data)

        if status != 0:
            funcname = Interface.cpu1_control_en.__name__
            print('{:}: Error enabling control. Error code {:}\r\n'.format(funcname, status))

        return status
    
