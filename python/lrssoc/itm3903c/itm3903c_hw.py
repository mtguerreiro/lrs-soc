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
        self.set_slope                  = 0  # Programming Guide Page 111 [SOURce:]EXTern:PROGram:CHANnel:MX <NR1>,<NRf>
        self.get_slope                  = 1  # Programming Guide Page 112 [SOURce:]EXTern:PROGram:CHANnel:MX? <NR1>
        self.get_version                = 2  # Programming Guide Page 30 SYSTem:VERSion?
        self.get_error                  = 3  # Programming Guide Page 31 SYSTem:ERRor?
        self.clear_error                = 4  # Programming Guide Page 32 SYSTem:CLEar
        self.set_output_status          = 5  # Programming Guide Page 118 OUTPut[:STATe] <CPD>
        self.get_output_status          = 6  # Programming Guide Page 119 OUTPut[:STATe]?
        self.set_analog_external_status = 7  # Programming Guide Page 109 [SOURce:]EXTern:PROGram[:STATe] <Bool>
        self.get_analog_external_status = 8  # Programming Guide Page 110 [SOURce:]EXTern:PROGram[:STATe]?
        self.set_offset                 = 9  # Programming Guide Page 112 [SOURce:]EXTern:PROGram:CHANnel:MB <NR1>,<NRf>
        self.get_offset                 = 10 # Programming Guide Page 113 [SOURce:]EXTern:PROGram:CHANnel:MB? <NR1>
        self.set_func_mode              = 11 # Programming Guide Page 51 [SOURce:]FUNCtion <CPD>                                  
        self.get_func_mode              = 12 # Programming Guide Page 52 [SOURce:]FUNCtion?
        self.set_volt_value             = 13 # Programming Guide Page 79 [SOURce:]VOLTage[:LEVel][:IMMediate][:AMPLitude] <NRf+> 
        self.set_curr_value             = 14 # Programming Guide Page 54 [SOURce:]CURRent[:LEVel][:IMMediate][:AMPLitude] <NRf+>

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


    def set_func_mode(self, func_mode):

        return self._set_func_mode(func_mode)

    
    def get_func_mode(self):

        return self._get_func_mode()


    def set_volt_value(self, value):
        return self._set_volt_value(value)

    
    def set_curr_value(self, value):

        return self._set_curr_value(value)

    
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
        size = len(version_b)
        str_format = '<{:}s'.format(size)
        version = struct.unpack(str_format, version_b)[0]

        string = version.decode()

        return (0, string)


    def _get_error(self):
        cmd = self._cmd.get_error

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, error_msg_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting error status. Error code {:}\r\n'.format(status))
            return (-1, status)

        size = len(error_msg_b)
        str_format = '<{:}s'.format(size)
        error_msg = struct.unpack(str_format, error_msg_b)[0]

        string = error_msg.decode()

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


    def _set_output_status(self, output_status):

        cmd = self._cmd.set_output_status

        if output_status is True:
            output_status = 1
        elif output_status is False:
            output_status = 0
        else:
            return(-1, 'output_status can only be True or False')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(output_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting output status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)       
        

    def _set_analog_external_status(self, analog_status):

        cmd = self._cmd.set_analog_external_status
        
        if analog_status is True:
            analog_status = 1
        elif analog_status is False:
            analog_status = 0
        else:
            return(-1, 'analog_status can only be True or False')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(analog_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting analog external status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)   


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
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


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
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, slope)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  

    
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
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


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
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0, offset)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_func_mode(self, func_mode):

        cmd = self._cmd.set_func_mode

        if func_mode == 'current':
            func = 0
        elif func_mode == 'voltage':
            func = 1
        else:
            return (-1, 'Mode can only be \'current\' or \'voltage\'')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(func, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting function mode. Error code {:}\r\n'.format(status))
            return (-1, status)        
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  

    
    def _get_func_mode(self):
        cmd = self._cmd.get_func_mode

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, func_mode_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting function mode. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        size = len(func_mode_b)
        str_format = '<{:}s'.format(size)
        func_mode = struct.unpack(str_format, func_mode_b)[0]

        string = func_mode.decode()
        string = func_mode.split(b'\x00')[0].decode()

        if string == 'VOLTage\n':
            string = 'voltage'
        elif string == 'CURRent\n':
            string = 'current'

        return (0, string)


    def _set_volt_value(self, value):
        cmd = self._cmd.set_volt_value

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage value. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  

    
    def _set_curr_value(self, value):
        cmd = self._cmd.set_curr_value

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage value. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  
