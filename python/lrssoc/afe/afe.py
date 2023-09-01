"""
Module ``afe``
==============

Application for the active rectifier platform

"""
import socket
import time
import struct
import lrssoc
#import enum

class Commands:
    """Just a list with the commands accepted by the platform. They must match
    the commands that are defined in the `soc_defs.h` file from the hardware.
    """
    def __init__(self):
        self.blink = 0
        self.adc_en = 1
        self.adc_config = 2
        self.pwm_config = 3
        
class AFE:
    """A class to provide access to the active rectifier platform.

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, comm_type='ethernet', settings={}):

        self.hwi = lrssoc.ocp.iface.Interface(comm_type='ethernet', settings=settings)
        
        self.cmd = Commands()

##    def blink(self, t=1000):
##        """Changes the blinking period of CPU0.
##
##        Parameters
##        ----------
##        t : int
##            Period, in milliseconds. By default, it is 1000.
##
##        Raises
##        ------
##        TypeError
##            If `t` is not of `int` type.
##
##        """
##        if type(t) is not int:
##            raise TypeError('`t` must be of int type.')
##
##        cmd = self.cmd.blink
##
##        tx_data = []        
##        tx_data.extend( lrssoc.conversions.u32_to_u8(t, msb=False) )
##
##        status, _ = self.hwp.request(cmd, tx_data)
##
##        if status < 0:
##            funcname = Interface.cpu0_blink.__name__
##            print('{:}: Error blink CPU0 LED. Error code {:}\r\n'.format(funcname, status))
##
##        return status


    def adc_en(self, en):
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
        
        cmd = self.cmd.adc_en

        if en == True:
            en = [1]
        else:
            en = [0]

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )

        status, _ = self.hwi.cs_hardware_if(0, tx_data)

        if status < 0:
            funcname = AFE.adc_en.__name__
            print('{:}: Error enabling ADC. Error code {:}\r\n'.format(funcname, status))
            
        return status

    
    def adc_config(self, spifreq=10):
        """Sets the ADC's SPI clock frequency.

        The clock will be divided by 2 x `freq`.

        Parameters
        ----------
        spifreq : int
            Clock division factor. By default, it is 10.

        Raises
        ------
        TypeError
            If `freq` is not of `int` type.

        """        
        cmd = self.cmd.adc_config

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(spifreq, msb=False) )

        status, _ = self.hwi.cs_hardware_if(0, tx_data)

        if status < 0:
            funcname = AFE.adc_config.__name__
            print('{:}: Error configuring ADC. Error code {:}\r\n'.format(funcname, status))
            
        return status
    

    def pwm_config(self, pwmfreq=10000):
        """Sets the PWM frequency. This is also the ADC's sampling frqeuency.

        The frequency will be given by MAIN_CLOCK / (2 x `freq`).

        Parameters
        ----------
        pwmfreq : int
            Clock division factor. By default, it is 10000.

        Raises
        ------
        TypeError
            If `freq` is not of `int` type.

        """        
        cmd = self.cmd.pwm_config

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(pwmfreq, msb=False) )

        status, _ = self.hwi.cs_hardware_if(0, tx_data)

        if status < 0:
            funcname = AFE.pwm_config.__name__
            print('{:}: Error configuring PWM. Error code {:}\r\n'.format(funcname, status))

        return status


    def set_controller(self, controller=0):
        """Sets the controller.

        Parameters
        ----------
        controller : int
            Controller.

        Raises
        ------
        TypeError
            If `freq` is not of `int` type.

        """
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(0, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(controller, msb=False) )

        status, _ = self.hwi.cs_controller_if(0, tx_data)

        if status < 0:
            funcname = AFE.set_controller.__name__
            print('{:}: Error setting the controller. Error code {:}\r\n'.format(funcname, status))
            
        return status
##
##
##    def cpu1_adc_error_read(self):
##        """Reads the ADC's error flag.
##
##        Returns
##        ------
##        int
##            Error flag.
##
##        """        
##        cmd = self.cmd.cpu1_adc_error_read
##        status, data = self.hwp.request(cmd)
##
##        if status < 0:
##            funcname = Interface.cpu1_adc_error_read.__name__
##            print('{:}: Error reading ADC error flag. Error code {:}\r\n'.format(cpu1_adc_error_read, status))
##            return (-1, status)
##
##        error = lrssoc.conversions.u8_to_u32(data, msb=False)
##
##        return (0, error)
##
##
##    def cpu1_adc_error_clear(self):
##        """Reads the ADC's error flag.
##
##        Returns
##        ------
##        int
##            Error flag.
##
##        """        
##        cmd = self.cmd.cpu1_adc_error_clear
##        
##        status, _ = self.hwp.request(cmd)
##
##        if status != 0:
##            funcname = Interface.cpu1_adc_error_clear.__name__   
##            print('{:}: Error clearing ADC error flag. Error code {:}\r\n'.format(funcname, status))
##
##        return status
##    
##
##    def cpu0_trace_start(self):
##        """Start recording data.
##
##        """        
##        cmd = self.cmd.cpu0_trace_start
##
##        status, _ = self.hwp.request(cmd)
##
##        if status < 0:
##            funcname = Interface.cpu0_trace_start.__name__
##            print('{:}: Error starting trace. Error code {:}\r\n'.format(funcname, status))
##
##        return status
##    
##
##    def cpu0_trace_read(self):
##        """Read recorded data.
##
##        """
##        cmd = self.cmd.cpu0_trace_read
##            
##        status, data = self.hwp.request(cmd)
##
##        if status < 0:
##            funcname = Interface.cpu0_trace_read.__name__
##            print('{:}: Error reading trace. Error code {:}\r\n'.format(funcname, status))
##            return (-1, status)
##        
##        return (0, data)
##
##
##    def cpu0_trace_read_tags(self):
##        """Reads traces' tags.
##
##        """
##        cmd = self.cmd.cpu0_trace_read_tags
##            
##        status, data = self.hwp.request(cmd)
##
##        if status < 0:
##            funcname = Interface.cpu0_trace_read_tag.__name__
##            print('{:}: Error reading traces\' tags. Error code {:}\r\n'.format(funcname, status))
##            return (-1, status)
##
##        # Split creates an empty string because of the last \x00. Thus, we
##        # save the tags only up to the last element.
##        tags = data.split(b'\x00')[:-1]
##        
##        return (0, tags)
##
##    
##    def cpu0_trace_size_set(self, size):
##        """Sets the number of bytes to save.
##
##        """
##        cmd = self.cmd.cpu0_trace_size_set
##
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
##        
##        status, _ = self.hwp.request(cmd, tx_data)
##
##        if status < 0:
##            funcname = Interface.cpu0_trace_size_set.__name__
##            print('{:}: Error setting trace size. Error code {:}\r\n'.format(funcname, status))
##
##        return status
##    
##
##    def cpu0_trace_size_read(self):
##        """Gets the number of bytes saved
##
##        """
##        cmd = self.cmd.cpu0_trace_size_read
##                
##        status, data = self.hwp.request(cmd)
##
##        if status < 0:
##            funcname = Interface.cpu0_trace_size_read.__name__
##            print('{:}: Error reading trace size. Error code {:}\r\n'.format(funcname, status))
##            return (-1, status)
##        
##        size = lrssoc.conversions.u8_to_u32(data, msb=False)
##
##        return (0, size)
##
##
##    def cpu1_control_en(self, en):
##        """Enables/disables the control algorithm.
##
##        Parameters
##        ----------
##        en : bool
##            If `True`, enables control. If `False`, disables control.
##
##        Raises
##        ------
##        TypeError
##            If `en` is not of `bool` type.
##
##        """        
##        if type(en) is not bool:
##            raise TypeError('`en` must be of bool type.')
##        
##        cmd = self.cmd.cpu0_control_en
##
##        if en == True:
##            en = [1]
##        else:
##            en = [0]
##
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )
##
##        status, _ = self.hwp.request(cmd, tx_data)
##
##        if status < 0:
##            funcname = Interface.cpu1_control_en.__name__
##            print('{:}: Error enabling control. Error code {:}\r\n'.format(funcname, status))
##
##        return status
    
