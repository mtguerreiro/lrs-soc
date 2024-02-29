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
        self.get_error = 3
        self.clear_error = 4
        self.set_output_status = 5
        self.get_output_status = 6
        self.set_analog_external_status = 7
        self.get_analog_external_status = 8
        self.set_offset = 9
        self.get_offset = 10 

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
    
    def set_offset(self, channel, offset):

        return self._set_offset(channel, offset)


    def get_offset(self, channel):

        return self._get_offset(channel)

    def get_version(self):

        return self._get_version()
    
    def get_error(self):

        return self._get_error()
    
    def clear_error(self):

        return self._clear_error()
    
    def set_output_status(self, set_status):

        return self._set_output_status(set_status)

    def get_output_status(self):

        return self._get_output_status()
    
    def set_analog_external_status(self, set_status):

        return self._set_analog_external_status(set_status)

    def get_analog_external_status(self):

        return self._get_analog_external_status()
    
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

    def _get_error(self):
        cmd = self._cmd.get_error

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, error_msg_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting error status. Error code {:}\r\n'.format(status))
            return (-1, status)

        error_msg = struct.unpack('<100s', error_msg_b)[0]

        string = error_msg.split(b'\x00')[0].decode()
        return (0, string)

    def _clear_error(self):
        cmd = self._cmd.clear_error

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error clearing error status. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)

    def _set_output_status(self, set_status):
        cmd = self._cmd.set_output_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(set_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting output status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        

        return (0,)

    def _set_analog_external_status(self, set_status):
        cmd = self._cmd.set_analog_external_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(set_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting analog external status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        

        return (0,)

    def _get_output_status(self):

        cmd = self._cmd.get_output_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, output_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting output status. Error code {:}\r\n'.format(status))
            return (-1, status)

        output_status = struct.unpack('<i', output_status)[0]
        return (0, output_status > 0)

    def _get_analog_external_status(self):

        cmd = self._cmd.get_analog_external_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, output_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting output status. Error code {:}\r\n'.format(status))
            return (-1, status)

        output_status = struct.unpack('<i', output_status)[0]
        return (0, output_status > 0)
    
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
    
    def _set_offset(self, channel, offset):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_offset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(channel, msb=False) )
        tx_data.extend( list(struct.pack('<f', offset)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting offset. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_offset(self, channel):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_offset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(channel, msb=False) )
        
        status, offset_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting offset. Error code {:}\r\n'.format(status))
            return (-1, status)

        offset = struct.unpack('<f', offset_b)[0]
        
        return (0, offset)
