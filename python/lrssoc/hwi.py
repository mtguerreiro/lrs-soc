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
        self.cpu0_blink = 0x00
        self.cpu1_blink = 0x01


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
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=True) )
        tx_data.extend( lrssoc.conversions.u16_to_u8(t, msb=True) )

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
        
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=True) )
        tx_data.extend( lrssoc.conversions.u16_to_u8(t, msb=True) )

        self.hwc.comm(tx_data)
