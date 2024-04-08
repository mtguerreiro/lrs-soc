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
    #28.03.2024
        self.get_voltage_measurement    = 15 # Programming Guide Page 280 MEASure[:SCALar]:VOLTage[:DC]?
        self.get_current_measurement    = 16 # Programming Guide Page 276 MEASure[:SCALar]:CURRent[:DC]?
        self.get_voltage_max            = 17 # Programming Guide Page 97 [SOURce:]VOLTage:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_voltage_min            = 18 # Programming Guide Page 99 [SOURce:]VOLTage:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_voltage_max            = 19 # Programming Guide Page 96 [SOURce:]VOLTage:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+>
        self.set_voltage_min            = 20 # Programming Guide Page 98 [SOURce:]VOLTage:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
    #05.04.2024
        self.get_power_max              = 21 # Programming Guide Page 100 [SOURce:]POWer:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_power_min              = 22 # Programming Guide Page 102 [SOURce:]POWer:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_power_max              = 23 # Programming Guide Page 100 [SOURce:]POWer:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+>
        self.set_power_min              = 24 # Programming Guide Page 101 [SOURce:]POWer:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
        self.get_current_max            = 25 # Programming Guide Page 73 [SOURce:]CURRent:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_current_min            = 26 # Programming Guide Page 75 [SOURce:]CURRent:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_current_max            = 27 # Programming Guide Page 72 [SOURce:]CURRent:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+> 
        self.set_current_min            = 28 # Programming Guide Page 74 [SOURce:]CURRent:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
    #08.04.2024
        self.get_voltage_protection_status = 29  # Programming Guide Page 86 [SOURce:]VOLTage[:OVER]:PROTection:STATe?
        self.set_voltage_protection_status = 30  # Programming Guide Page 85 [SOURce:]VOLTage[:OVER]:PROTection:STATe <Bool>
        self.get_power_protection_status   = 31  # Programming Guide Page 103 [SOURce:]POWer[:OVER]:PROTection:STATe?
        self.set_power_protection_status   = 32  # Programming Guide Page 103 [SOURce:]POWer[:OVER]:PROTection:STATe <Bool>
        self.get_current_protection_status = 33  # Programming Guide Page 61 [SOURce:]CURRent[:OVER]:PROTection:STATe?
        self.set_current_protection_status = 34  # Programming Guide Page 61 [SOURce:]CURRent[:OVER]:PROTection:STATe <Bool>
        self.get_voltage_slew_rate         = 35  # Programming Guide Page 81 [SOURce:]VOLTage:SLEW[:BOTH]? [MINimum|MAXimum|DEFault]
        self.set_voltage_slew_rate         = 36  # Programming Guide Page 80 [SOURce:]VOLTage:SLEW[:BOTH] <NRf+>
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

    #28.03.2024
    def get_voltage_measurement(self):

        return self._get_voltage_measurement()

    def get_current_measurement(self):

        return self._get_current_measurement()

    def get_voltage_max(self):

        return self._get_voltage_max()

    def get_voltage_min(self):

        return self._get_voltage_min()
    
    def set_voltage_max(self, MAX):

        return self._set_voltage_max(MAX)
    
    def set_voltage_min(self, MIN):

        return self._set_voltage_min(MIN)
    
    def get_power_max(self):

        return self._get_power_max()

    def get_power_min(self):

        return self._get_power_min()
    
    def set_power_max(self, MAX):

        return self._set_power_max(MAX)
    
    def set_power_min(self, MIN):

        return self._set_power_min(MIN)
    
    def get_current_max(self):

        return self._get_current_max()

    def get_current_min(self):

        return self._get_current_min()
    
    def set_current_max(self, MAX):

        return self._set_current_max(MAX)
    
    def set_current_min(self, MIN):

        return self._set_current_min(MIN)

    
    #08.04.2024
    def get_voltage_protection_status(self):

        return self._get_voltage_protection_status()
    
    
    def set_voltage_protection_status(self, set_status):

        return self._set_voltage_protection_status(set_status)


    def get_power_protection_status(self):

        return self._get_power_protection_status()
    
    
    def set_power_protection_status(self, set_status):

        return self._set_power_protection_status(set_status)


    def get_current_protection_status(self):

        return self._get_current_protection_status()
    
    
    def set_current_protection_status(self, set_status):

        return self._set_current_protection_status(set_status)
        
    def get_voltage_slew_rate(self):

        return self._get_voltage_slew_rate()

    def set_voltage_slew_rate(self, value):

        return self._set_voltage_slew_rate(value)
    
    #
    
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
            

#28.03.2024
    def _get_voltage_measurement(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_voltage_measurement

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, voltage_measurement_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting voltage measurement. Error code {:}\r\n'.format(status))
            return (-1, status)

        voltage_measurement = struct.unpack('<f', voltage_measurement_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, voltage_measurement)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)

    def _get_current_measurement(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_current_measurement

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, current_measurement_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting current measurement. Error code {:}\r\n'.format(status))
            return (-1, status)

        current_measurement = struct.unpack('<f', current_measurement_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, current_measurement)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)
            
    def _get_voltage_max(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_voltage_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        #tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, voltage_max_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting voltage_max. Error code {:}\r\n'.format(status))
            return (-1, status)

        voltage_max = struct.unpack('<f', voltage_max_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, voltage_max)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _get_voltage_min(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_voltage_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
       # tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, voltage_min_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting voltage_min. Error code {:}\r\n'.format(status))
            return (-1, status)

        voltage_min = struct.unpack('<f', voltage_min_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, voltage_min)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)

            
    def _set_voltage_max(self, value):
        cmd = self._cmd.set_voltage_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage max. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_voltage_min(self, value):
        cmd = self._cmd.set_voltage_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage min. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)
            
    def _get_power_max(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_power_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        #tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, power_max_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting power max. Error code {:}\r\n'.format(status))
            return (-1, status)

        power_max = struct.unpack('<f', power_max_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, power_max)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _get_power_min(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_power_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
       # tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, power_min_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting power min. Error code {:}\r\n'.format(status))
            return (-1, status)

        power_min = struct.unpack('<f', power_min_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, power_min)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)

            
    def _set_power_max(self, value):
        cmd = self._cmd.set_power_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting power max. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_power_min(self, value):
        cmd = self._cmd.set_power_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting power min. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)

    def _get_current_max(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_current_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        #tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, current_max_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting current max. Error code {:}\r\n'.format(status))
            return (-1, status)

        current_max = struct.unpack('<f', current_max_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, current_max)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _get_current_min(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_current_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
       # tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, current_min_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting current min. Error code {:}\r\n'.format(status))
            return (-1, status)

        current_min = struct.unpack('<f', current_min_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, current_min)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)

            
    def _set_current_max(self, value):
        cmd = self._cmd.set_current_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting current max. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_current_min(self, value):
        cmd = self._cmd.set_current_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting current min. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)
#08.04.2024
    def _get_voltage_protection_status(self):

        cmd = self._cmd.get_voltage_protection_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, output_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage protection status. Error code {:}\r\n'.format(status))
            return (-1, status)

        output_status = struct.unpack('<i', output_status)[0]

        return (0, output_status > 0)

        
    def _set_voltage_protection_status(self, volt_prot_status):

        cmd = self._cmd.set_voltage_protection_status
        
        if volt_prot_status is True:
            volt_prot_status = 1
        elif volt_prot_status is False:
            volt_prot_status = 0
        else:
            return(-1, 'volt_prot_status can only be True or False')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(volt_prot_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage protection status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)   


    def _get_power_protection_status(self):

        cmd = self._cmd.get_power_protection_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, output_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting power protection status. Error code {:}\r\n'.format(status))
            return (-1, status)

        output_status = struct.unpack('<i', output_status)[0]

        return (0, output_status > 0)

        
    def _set_power_protection_status(self, pow_prot_status):

        cmd = self._cmd.set_power_protection_status
        
        if pow_prot_status is True:
            pow_prot_status = 1
        elif pow_prot_status is False:
            pow_prot_status = 0
        else:
            return(-1, 'pow_prot_status can only be True or False')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(pow_prot_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting power protection status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)   


    def _get_current_protection_status(self):

        cmd = self._cmd.get_current_protection_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, output_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting current protection status. Error code {:}\r\n'.format(status))
            return (-1, status)

        output_status = struct.unpack('<i', output_status)[0]

        return (0, output_status > 0)

        
    def _set_current_protection_status(self, curr_prot_status):

        cmd = self._cmd.set_current_protection_status
        
        if curr_prot_status is True:
            curr_prot_status = 1
        elif curr_prot_status is False:
            curr_prot_status = 0
        else:
            return(-1, 'curr_prot_status can only be True or False')

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(curr_prot_status, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting current protection status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)   


    def _get_voltage_slew_rate(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_voltage_slew_rate

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        #tx_data.extend( lrssoc.conversions.u32_to_u8(MAX, msb=False) )
        
        status, voltage_slew_rate_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting voltage slew rate. Error code {:}\r\n'.format(status))
            return (-1, status)

        voltage_slew_rate = struct.unpack('<f', voltage_slew_rate_b)[0]
        
        err_status, err_message = self.get_error()
        if "No error" in err_message:
            return (0, voltage_slew_rate)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_voltage_slew_rate(self, value):
        cmd = self._cmd.set_voltage_slew_rate

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting voltage slew rate. Error code {:}\r\n'.format(status))
            return (-1, status)
    
        err_status, err_message = self.get_error()

        if "No error" in err_message:
            return (0,)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  

