"""
Module ``itm3903c_hw``
======================


"""
import lrssoc
import struct

class Commands:
    """
    """
    def __init__(self):
        self.set_slope = 0
        self.get_slope = 1
        self.get_version = 2

class MeasGains:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 4
        data = struct.unpack(fmt, data)

        gains = {
            'v_gain':      data[0],  'v_ofs':      data[1],
            'i_gain':      data[2],  'i_ofs':      data[3],
            }

        return gains

    def encode(self, gains):
        data = [gains['v_gain'],      gains['v_ofs'],
                gains['i_gain'],      gains['i_ofs'],
                ]
        fmt = '<' + 'f' * 4
        data = struct.pack(fmt, *data)

        return data

        
class Hw:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id):

        self._cmd = Commands()
        self._ocp_if = ocp_if
        self._cs_id = cs_id


    def set_slope(self, channel, slope):

        return self._set_slope(channel, slope)


    def get_slope(self, channel):

        return self._get_slope(channel)

    def get_version(self):

        return self._get_version()
    
    def _get_version(self):
        """
        Returns the version of the firmware of the power supply

        """

        cmd = self._cmd.get_version

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, version_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting version of the firmware. Error code {:}\r\n'.format(status))
            return (-1, status)

        version = struct.unpack('<100s', version_b)[0]

        string = version.split(b'\x00')[0].decode()
        return (0, string)


    def _set_slope(self, channel, slope):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_slope

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(channel, msb=False) )
        tx_data.extend( list(struct.pack('<f', slope)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting slope. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_slope(self, channel):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_slope

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(channel, msb=False) )
        
        status, slope_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting slope. Error code {:}\r\n'.format(status))
            return (-1, status)

        slope = struct.unpack('<f', slope_b)[0]
        
        return (0, slope)
