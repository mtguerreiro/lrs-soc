"""
Module ``itm3903c_hw``
======================


"""
import lrssoc
import struct
import json

class DigitalCommands:
    """
    """
    def __init__(self):
        self.set_slope                      = 0  # Programming Guide Page 111 [SOURce:]EXTern:PROGram:CHANnel:MX <NR1>,<NRf>
        self.get_slope                      = 1  # Programming Guide Page 112 [SOURce:]EXTern:PROGram:CHANnel:MX? <NR1>
        self.get_version                    = 2  # Programming Guide Page 30 SYSTem:VERSion?
        self.get_error                      = 3  # Programming Guide Page 31 SYSTem:ERRor?
        self.clear_error                    = 4  # Programming Guide Page 32 SYSTem:CLEar
        self.set_output_status              = 5  # Programming Guide Page 118 OUTPut[:STATe] <CPD>
        self.get_output_status              = 6  # Programming Guide Page 119 OUTPut[:STATe]?
        self.set_analog_external_status     = 7  # Programming Guide Page 109 [SOURce:]EXTern:PROGram[:STATe] <Bool>
        self.get_analog_external_status     = 8  # Programming Guide Page 110 [SOURce:]EXTern:PROGram[:STATe]?
        self.set_offset                     = 9  # Programming Guide Page 112 [SOURce:]EXTern:PROGram:CHANnel:MB <NR1>,<NRf>
        self.get_offset                     = 10 # Programming Guide Page 113 [SOURce:]EXTern:PROGram:CHANnel:MB? <NR1>
        self.set_func_mode                  = 11 # Programming Guide Page 51 [SOURce:]FUNCtion <CPD>                                  
        self.get_func_mode                  = 12 # Programming Guide Page 52 [SOURce:]FUNCtion?
        self.set_volt_value                 = 13 # Programming Guide Page 79 [SOURce:]VOLTage[:LEVel][:IMMediate][:AMPLitude] <NRf+> 
        self.set_curr_value                 = 14 # Programming Guide Page 54 [SOURce:]CURRent[:LEVel][:IMMediate][:AMPLitude] <NRf+>
        self.get_voltage_measurement        = 15 # Programming Guide Page 280 MEASure[:SCALar]:VOLTage[:DC]?
        self.get_current_measurement        = 16 # Programming Guide Page 276 MEASure[:SCALar]:CURRent[:DC]?
        self.get_voltage_max                = 17 # Programming Guide Page 97 [SOURce:]VOLTage:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_voltage_min                = 18 # Programming Guide Page 99 [SOURce:]VOLTage:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_voltage_max                = 19 # Programming Guide Page 96 [SOURce:]VOLTage:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+>
        self.set_voltage_min                = 20 # Programming Guide Page 98 [SOURce:]VOLTage:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
        self.get_power_max                  = 21 # Programming Guide Page 100 [SOURce:]POWer:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_power_min                  = 22 # Programming Guide Page 102 [SOURce:]POWer:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_power_max                  = 23 # Programming Guide Page 100 [SOURce:]POWer:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+>
        self.set_power_min                  = 24 # Programming Guide Page 101 [SOURce:]POWer:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
        self.get_current_max                = 25 # Programming Guide Page 73 [SOURce:]CURRent:LIMit[:POSitive][:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.get_current_min                = 26 # Programming Guide Page 75 [SOURce:]CURRent:LIMit:NEGative[:IMMediate][:AMPLitude]? [MINimum|MAXimum|DEFault]
        self.set_current_max                = 27 # Programming Guide Page 72 [SOURce:]CURRent:LIMit[:POSitive][:IMMediate][:AMPLitude] <NRf+> 
        self.set_current_min                = 28 # Programming Guide Page 74 [SOURce:]CURRent:LIMit:NEGative[:IMMediate][:AMPLitude] <NRf+>
        self.get_voltage_protection_status  = 29 # Programming Guide Page 86 [SOURce:]VOLTage[:OVER]:PROTection:STATe?
        self.set_voltage_protection_status  = 30 # Programming Guide Page 85 [SOURce:]VOLTage[:OVER]:PROTection:STATe <Bool>
        self.get_power_protection_status    = 31 # Programming Guide Page 103 [SOURce:]POWer[:OVER]:PROTection:STATe?
        self.set_power_protection_status    = 32 # Programming Guide Page 103 [SOURce:]POWer[:OVER]:PROTection:STATe <Bool>
        self.get_current_protection_status  = 33 # Programming Guide Page 61 [SOURce:]CURRent[:OVER]:PROTection:STATe?
        self.set_current_protection_status  = 34 # Programming Guide Page 61 [SOURce:]CURRent[:OVER]:PROTection:STATe <Bool>
        self.get_voltage_slew_rate          = 35 # Programming Guide Page 81 [SOURce:]VOLTage:SLEW[:BOTH]? [MINimum|MAXimum|DEFault]
        self.set_voltage_slew_rate          = 36 # Programming Guide Page 80 [SOURce:]VOLTage:SLEW[:BOTH] <NRf+>

        
class DigitalHw:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id):

        self._cmd = DigitalCommands()
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
    

    def config_slope_offset(self, channel, a_in, a_out):

        mx = (a_out[1] - a_out[0]) / (a_in[1] - a_in[0])
        mb = a_out[1] - a_in[1] * mx

        status = self.set_slope(channel, mx)
        if status[0] < 0:
            print('Error setting slope of channel {:}. Status: {:}, error {:}'.format(channel, status[0], status[1]))
            return status
        
        status = self.set_offset(channel, mb)
        if status[0] < 0:
            print('Error setting offset of channel {:}. Status: {:}, error {:}'.format(channel, status[0], status[1]))
            return status

        return (0, (mx, mb))

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
        """
        Returns the error information of the power supply

        """
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
        """
        Clears the error queue

        """    
        cmd = self._cmd.clear_error

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error clearing error status. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def _set_output_status(self, output_status):
        """
        The output status function enables or disables the output.

        Parameters
        ----------
        output_status : bool
            If set to true, output is enabled. If false, output is disabled.

        """    
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
        """
        enables or disables the analog external.
        
        Parameters
        ----------
        analog_status : bool
            If set to true, analog external is enabled. If false, analog external is disabled.

        """    
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
        """
        Returns the status of the output

        """
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
        """
        Returns the status of the external analog function

        """
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
        set the linear calibration coefficient of the channel (1/2/3)
        
        Parameters
        ----------
        channel: uint32, can be set to 1,2,3
        slope: float
            value range of slope (i.e. the linear calibration coefficient): min(-9000) to max(9000)
            
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
        query the linear calibration coefficient of the channel (1/2/3)
        
        Parameters
        ----------
        channel: uint32, can be set to 1,2,3

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
        set the offset calibration coefficient of the channel (1/2/3)
        
        Parameters
        ----------
        channel: uint32, can be set to 1,2,3
        offset: float
            value range of offset (i.e. offset calibration coefficient): min(-9000) to max(9000)
          
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
        query the offset calibration coefficient of the channel (1/2/3)
        
        Parameters
        ----------
        channel: uint32, can be set to 1,2,3

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

        if err_status < 0 :
            print('Error getting supply error. Error code {:}\r\n'.format(err_status))
            return (-1, err_status)
        
        if "No error" in err_message:
            return (0, offset)
        else:
            if err_status < 0:
                return (-1, err_status)
            else: 
                return (-1, err_message)  


    def _set_func_mode(self, func_mode):
        """
        set the working mode of the power supply
        
        Parameters
        ----------
        func_mode: uint32
            If FUNCMODE is 0, then current mode is set. If 1, the voltage mode.

        """    
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
        """
        Returns the working mode of the power suply

        """
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
        """
        set the output voltage value Vs in CV priority mode
        
        Parameters
        ----------
        value: float, range: MIN to MAX
        
        """    
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
        """
        set the output current value Is in CC priority mode
        
        Parameters
        ----------
        value: float, range: MIN to MAX
        
        """    
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
            

    def _get_voltage_measurement(self):
        """
        Returns the averaged voltage measurement

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
        Returns the averaged current measurement

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
        Returns the voltage upper limit value in CC priority mode

        """   
        cmd = self._cmd.get_voltage_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        Returns the voltage lower limit value VI in CC priority mode

        """   
        cmd = self._cmd.get_voltage_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        """
        set the voltage upper limit value Vh in CC priority mode
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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
        """
        set the voltage lower limit value Vl in CC priority mode
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """      
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
        Returns the power upper limit value

        """     
        cmd = self._cmd.get_power_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        Returns the power lower limit value P-

        """  
        cmd = self._cmd.get_power_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        """
        set the power upper limit value P+
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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
        """
        set the power lower limit value P-
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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
        Returns the current upper limit value in CV priority mode

        """     
        cmd = self._cmd.get_current_max

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        Returns the current lower limit value I- in CV priority mode

        """     
        cmd = self._cmd.get_current_min

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        """
        set the current upper limit value I+ in CV priority mode
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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
        """
        set the current lower limit value I- in CV priority mode
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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


    def _get_voltage_protection_status(self):
        """
        Returns the status of overvoltage protection

        """     
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
        """
        enables or disables the overvoltage protection.
        
        Parameters
        ----------
        volt_prot_status : bool
            If set to true, overvoltage protection is enabled. If false, overvoltage protection is disabled.

        """   
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
        """
        Returns the status of power protection

        """     
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
        """
        enables or disables the power protection.
        
        Parameters
        ----------
        pow_prot_status : bool
            If set to true, power protection is enabled. If false, power protection is disabled.

        """      
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
        """
        Returns the status of overcurrent protection

        """     
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
        """
        enables or disables the overcurrent protection.
        
        Parameters
        ----------
        curr_prot_status : bool
            If set to true, overcurrent protection is enabled. If false, overcurrent protection is disabled.

        """   
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
        Returns the voltage rise and fall times

        """     
        cmd = self._cmd.get_voltage_slew_rate

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
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
        """
        set the voltage rise and fall time
        
        Parameters
        ----------
        value: float, MIN to MAX
        
        """    
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


class AnalogCommands:
    """
    """
    def __init__(self):
        self.set_sampling_status    = 0
        self.get_sampling_status    = 1
        self.set_sampling_freq      = 2
        self.get_sampling_freq      = 3
        self.set_dac1_offset        = 4
        self.set_dac1_adj           = 5
        self.set_dac23_a2           = 6
        self.set_dac23_a3           = 7
        self.set_dac_cal_data       = 8
        self.get_dac_cal_data       = 9
        self.set_adc_cal_data       = 10
        self.get_adc_cal_data       = 11
        self.get_alpha_values       = 12
        self.set_alpha_values       = 13
        self.get_neighbor_ips       = 14
        self.set_neighbor_ips       = 15
 

class MeasGains:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 4
        data = struct.unpack(fmt, data)
        
        gains = {
            'v_gain':     data[0],
            'v_ofs':      data[1],
            
            'i_gain':     data[2],
            'i_ofs':      data[3],
            }
        
        return gains

    def encode(self, gains):
        data = [gains['v_gain'],
                gains['v_ofs'],
                
                gains['i_gain'],
                gains['i_ofs'],
                ]
        
        fmt = '<' + 'f' * 4
        data = struct.pack(fmt, *data)

        return data

class AlphaValues: 
    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 2
        data = struct.unpack(fmt, data)
        
        gains = {
            'v':      data[0],
            'i':      data[1],
            }
        return gains

    def encode(self, gains):
        data = [gains['v'],
                gains['i'],
                ]
        
        fmt = '<' + 'f' * 2
        data = struct.pack(fmt, *data)

        return data
class DacCalData:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 8
        
        data = struct.unpack(fmt, data)

        gains = {
            'dac1_adj_gain':        data[0],
            'dac1_adj_offset':      data[1],

            'dac1_offset_gain':     data[2],
            'dac1_offset_offset':   data[3],

            'dac2_gain':            data[4],
            'dac2_offset':          data[5],

            'dac3_gain':            data[6],
            'dac3_offset':          data[7],
            }

        return gains
    
    def encode(self, gains):
        data = [
            gains['dac1_adj_gain'],
            gains['dac1_adj_offset'],
            gains['dac1_offset_gain'],
            gains['dac1_offset_offset'],
            gains['dac2_gain'],
            gains['dac2_offset'],
            gains['dac3_gain'],
            gains['dac3_offset'],
                ]
        
        fmt = '<' + 'f' * 8
        data = struct.pack(fmt, *data)

        return data

class BoardComm:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 5
       
        data = struct.unpack(fmt, data)
        gains = {
            'ip1':  data[0],
            'ip2':  data[1],
            'ip3':  data[2],
            'ip4':  data[3],
            'ip5':  data[4],
            }

        return gains
    
    def encode(self, gains):
        data = [
            gains['ip1'],
            gains['ip2'],
            gains['ip3'],
            gains['ip4'],
            gains['ip5'],
            ]
        fmt = '<' + 'f' * 5
        data = struct.pack(fmt, *data)

        return data
        

    
class AnalogHw:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id):

        self._cmd = AnalogCommands()
        self._ocp_if = ocp_if
        self._cs_id = cs_id


    def set_sampling_status(self, status):
        """Sets the sampling status.

        Setting status to True enables sampling, while setting it to False
        disables it.

        Parameters
        ----------
        status : bool
            Sampling status

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """
        
        return self._set_sampling_status( int(bool(status)) )


    def get_sampling_status(self):
        """Gets the sampling status.
        
        Returns
        -------
        tuple
            Returns a tuple of the form `(status, samp_status)`. If the command
            was executed successfully, `status` is 0 and `samp_status` is the
            sampling status. Otherwise, `status` is an error code and
            `samp_status` contains additional error information.
        """
        
        return self._get_sampling_status()

    
    def set_sampling_freq(self, freq):
        """Sets the sampling frequency. `freq` must be an integer in units of Hz.

        Parameters
        ----------
        freq : integer
            Sampling frequency.

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """
        
        return self._set_sampling_freq(int(freq))


    def get_sampling_freq(self):
        """Gets the sampling frequency. `freq` is returned as integer in units of Hz.
        
        Returns
        -------
        tuple
            Returns a tuple of the form `(status, freq)`. If the command was
            executed successfully, `status` is 0 and `freq` is the frequency,
            in Hz. Otherwise, `status` is an error code and `freq` contains
            additional error information.
        """
        
        return self._get_sampling_freq()
    

    def set_dac1_offset(self, offset):
        """Sets offset of DAC channel 1. `offset` is in volts.


        Parameters
        ----------
        offset : float
            Offset, in volts.

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """
        
        return self._set_dac1_offset(float(offset))


    def set_dac1_adj(self, adj):
        """Sets adjust of DAC channel 1. `adj` is in volts.


        Parameters
        ----------
        adj : float
            Adjust, in volts.

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """
        
        return self._set_dac1_adj(float(adj))


    def set_dac23_a2(self, value):
        """Sets value of DAC channel a2. `value` is in volts.
        Parameters
        ----------
        value : float
            Value, in volts.
        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """

        return self._set_dac23_a2(float(value))


    def set_dac23_a3(self, value):
        """Sets value of DAC channel a3. `value` is in volts.
        Parameters
        ----------
        value : float
            Value, in volts.
        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """

        return self._set_dac23_a3(float(value))


    def set_dac_cal_data(self, data):
        """Sets the calibration data for the DAC channels.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """

        return self._set_dac_cal_data(data)
    

    def get_dac_cal_data(self):
        """Gets the calibration data for the DAC channels.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, data)`. If the command was
            executed successfully, `status` is 0 and `data` is the calibration
            data of the board. Otherwise, `status` is negative and `error` is
            an error code.
        """

        return self._get_dac_cal_data()
    
    def set_adc_cal_data(self, data):
        """Sets the calibration data for the ADC channels.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """

        return self._set_adc_cal_data(data)
    
    def get_adc_cal_data(self):
        """Gets the calibration data for the ADC channels.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, data)`. If the command was
            executed successfully, `status` is 0 and `data` is the calibration
            data of the board. Otherwise, `status` is negative and `error` is
            an error code.
        """

        return self._get_adc_cal_data()
    
    def set_alpha_values(self, data):
        """Sets the alpha values for filtering.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, error)`. If the command was
            executed successfully, `status` is 0 and `error` is empty.
            Otherwise, `status` is an error code and `error` is non-empty.
        """

        return self._set_alpha_values(data)
    
    def get_alpha_values(self):
        """Gets the alpha values for filtering.

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, data)`. If the command was
            executed successfully, `status` is 0 and `data` is the calibration
            data of the board. Otherwise, `status` is negative and `error` is
            an error code.
        """

        return self._get_alpha_values()
    
    def set_neighbor_ips(self, data):
        """Sets the neigbors ip addresses

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, data)`. If the command was
            executed successfully, `status` is 0 and `data` is the calibration
            data of the board. Otherwise, `status` is negative and `error` is
            an error code.
        """

        return self._set_neighbor_ips(data)
    
    def get_neighbor_ips(self):
        """Gets the neigbors ip addresses

        Parameters
        ----------

        Returns
        -------
        tuple
            Returns a tuple of the form `(status, data)`. If the command was
            executed successfully, `status` is 0 and `data` is the calibration
            data of the board. Otherwise, `status` is negative and `error` is
            an error code.
        """

        return self._get_neighbor_ips()
    

    def _set_sampling_status(self, status):
        cmd = self._cmd.set_sampling_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(status, msb=False) )


        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the sampling status. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)

    
    def _get_sampling_status(self):
        cmd = self._cmd.get_sampling_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting sampling status. Error code {:}\r\n'.format(status))
            return (-1, status)

        status = lrssoc.conversions.u8_to_u32(freq, msb=False)
        
        return (0, bool(status))
    

    def _set_sampling_freq(self, freq):
        cmd = self._cmd.set_sampling_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the sampling freq. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)

    
    def _get_sampling_freq(self):
        cmd = self._cmd.get_sampling_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting sampling frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = lrssoc.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)
    

    def _set_dac1_offset(self, offset):
        cmd = self._cmd.set_dac1_offset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', offset)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting offset of DAC 1. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)
    

    def _set_dac1_adj(self, adj):
        cmd = self._cmd.set_dac1_adj

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', adj)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting adj of DAC 1. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def _set_dac23_a2(self, value):
        cmd = self._cmd.set_dac23_a2

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting a2 of DAC 23. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def _set_dac23_a3(self, value):
        cmd = self._cmd.set_dac23_a3

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', value)) )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting a3 of DAC 23. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def _set_dac_cal_data(self, data):
        cmd = self._cmd.set_dac_cal_data

        data_b = DacCalData().encode(data)

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( data_b )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting dac cal data. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)
    
    def _set_adc_cal_data(self, data):
        cmd = self._cmd.set_adc_cal_data

        data_b = MeasGains().encode(data)

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( data_b )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting adc cal data. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def _get_dac_cal_data(self):
        cmd = self._cmd.get_dac_cal_data

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, data_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting dac cal data. Error code {:}\r\n'.format(status))
            return (-1, status)

        data = DacCalData().decode(data_b)
        
        return (0, data)
    
    def _get_adc_cal_data(self):
        cmd = self._cmd.get_adc_cal_data

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, data_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting adc cal data. Error code {:}\r\n'.format(status))
            return (-1, status)

        data = MeasGains().decode(data_b)
        
        return (0, data)
    
    def _set_alpha_values(self, data):
        cmd = self._cmd.set_alpha_values

        data_b = AlphaValues().encode(data)

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( data_b )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting alpha values. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)
    
    def _get_alpha_values(self):
        
        cmd = self._cmd.get_alpha_values

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, data_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting alpha values. Error code {:}\r\n'.format(status))
            return (-1, status)

        data = AlphaValues().decode(data_b)
        
        return (0, data)
    
    def _set_neighbor_ips(self, data):
        cmd = self._cmd.set_neighbor_ips

        data_b = BoardComm().encode(data)
        
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( data_b )

        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting neighbor ips. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)

    def _get_neighbor_ips(self):
        
        cmd = self._cmd.get_neighbor_ips

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, data_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)
        
        if status < 0:
            print('Error getting neighbor ips. Error code {:}\r\n'.format(status))
            return (-1, status)

        data = BoardComm().decode(data_b)
        
        return (0, data)

