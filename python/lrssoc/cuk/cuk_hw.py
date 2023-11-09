"""
Module ``cuk_hw``
=====================


"""
import lrssoc
import struct

class Commands:
    """
    """
    def __init__(self):
        self.set_pwm_reset = 0
        self.get_pwm_reset = 1

        self.set_pwm_output_enable = 2
        self.get_pwm_output_enable = 3

        self.set_pwm_ovf_trigger_enable = 4
        self.get_pwm_ovf_trigger_enable = 5
        
        self.set_pwm_freq = 6
        self.get_pwm_freq = 7
        
        self.set_pwm_duty = 8
        self.get_pwm_duty = 9
        
        self.set_pwm_dead_time = 10
        self.get_pwm_dead_time = 11
        
        self.set_adc_enable = 12
        self.get_adc_enable = 13
        
        self.set_adc_manual_trigger = 14
        self.get_adc_manual_trigger = 15
        
        self.set_adc_int_enable = 16
        self.get_adc_int_enable = 17
        
        self.set_adc_spi_freq = 18
        self.get_adc_spi_freq = 19

        self.set_load_switch = 20
        self.get_load_switch = 21

        self.set_output_switch = 22
        self.get_output_switch = 23

        self.set_meas_gains = 24
        self.get_meas_gains = 25

        self.clear_status = 26
        self.get_status = 27


class MeasGains:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 20
        data = struct.unpack(fmt, data)

        gains = {
            'i_i_gain':      data[0],  'i_i_ofs':      data[1],
            'i_1_gain':      data[2],  'i_1_ofs':      data[3],
            'v_in_gain':     data[4],  'v_in_ofs':     data[5],
            'v_dc_gain':     data[6],  'v_dc_ofs':     data[7],
            'v_1_gain':      data[8],  'v_1_ofs':      data[9],
            'i_o_gain':      data[10], 'i_o_ofs':      data[11],
            'i_2_gain':      data[12], 'i_2_ofs':      data[13],
            'v_out_gain':    data[14], 'v_out_ofs':    data[15],
            'v_dc_out_gain': data[16], 'v_dc_out_ofs': data[17],
            'v_2_gain':      data[18], 'v_2_ofs':      data[19],
            }

        return gains

    def encode(self, gains):
        data = [gains['i_i_gain'],      gains['i_i_ofs'],
                gains['i_1_gain'],      gains['i_1_ofs'],
                gains['v_in_gain'],     gains['v_in_ofs'],
                gains['v_dc_gain'],     gains['v_dc_ofs'],
                gains['v_1_gain'],      gains['v_1_ofs'],
                gains['i_o_gain'],      gains['i_o_ofs'],
                gains['i_2_gain'],      gains['i_2_ofs'],
                gains['v_out_gain'],    gains['v_out_ofs'],
                gains['v_dc_out_gain'], gains['v_dc_out_ofs'],
                gains['v_2_gain'],      gains['v_2_ofs']
                ]
        fmt = '<' + 'f' * 20
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
    def __init__(self, ocp_if, cs_id, base_clock=100000000):

        self._cmd = Commands()
        self._ocp_if = ocp_if
        self._cs_id = cs_id

        self._base_clock = base_clock

        self._period = 500


    def set_pwm_reset(self, reset):

        return self._set_pwm_reset(reset)


    def get_pwm_reset(self):

        return self._get_pwm_reset()
    

    def set_pwm_output_enable(self, enable):

        return self._set_pwm_output_enable(enable)


    def get_pwm_output_enable(self):

        return self._get_pwm_output_enable()
    

    def set_pwm_ovf_trigger_enable(self, enable):

        return self._set_pwm_ovf_trigger_enable(enable)


    def get_pwm_ovf_trigger_enable(self):

        return self._get_pwm_ovf_trigger_enable()
    

    def set_pwm_frequency(self, freq):
        """Sets PWM frequency, in Hz.
        """
        return self._set_pwm_freq(int(freq))


    def get_pwm_frequency(self):
        """Gets PWM frequency, in Hz.
        """
        status, freq = self._get_pwm_freq()
        if status != 0:
            return (-1, status)
        
        return (status, int(freq))
    

    def set_pwm_duty(self, duty):
        """Sets PWM duty (0 < duty < 1).
        """
        return self._set_pwm_duty(float(duty))


    def get_pwm_duty(self):
        """Gets PWM duty, in percentage.
        """
        status, duty = self._get_pwm_duty()
        if status != 0:
            return(-1, status)
        
        return (0, float(duty))
    

    def set_pwm_dead_time(self, dead_time):
        """Sets PWM dead time, in ns.
        """
        return self._set_pwm_dead_time(float(int(dead_time) * 1e-9))


    def get_pwm_dead_time(self):
        """Gets PWM dead time, in ns.
        """
        status, dt = self._get_pwm_dead_time()
        if status != 0:
            return (-1, status)

        return (0, round(float(dt / 1e-9)))
    

    def set_adc_enable(self, enable):
        """Enables the ADC.
        """

        return self._set_adc_enable(enable)


    def get_adc_enable(self):
        """Get ADC enable status.
        """
        return self._get_adc_enable()
    

    def set_adc_trigger(self):
        """Triggers the ADC for a single conversion.
        """

        status = self._set_adc_manual_trigger(0)
        if status[0] != 0:
            return (status, err)
        
        status = self._set_adc_manual_trigger(1)
        if status[0] != 0:
            return (status, err)
        
        return self._set_adc_manual_trigger(0)


    def get_adc_trigger(self):
        """
        """
        return self._get_adc_manual_trigger()


    def set_adc_int_enable(self, enable):
        """Enables ADC interrupt.
        """
        return self._set_adc_int_enable(enable)


    def get_adc_int_enable(self):
        """Gets ADC interrupt status.
        """
        return self._get_adc_int_enable()
    
    
    def set_adc_spi_freq(self, freq):
        """Sets ADC SPI clock frequency, in Hz.
        """
        return self._set_adc_spi_freq(int(freq))


    def get_adc_spi_freq(self):
        """Gets ADC SPI clock frequency, in Hz.
        """
        status, freq = self._get_adc_spi_freq()
        if status != 0:
            return (-1, status)
        
        return (status, int(freq))


    def set_load_switch(self, state):
        """Sets the load switch.
        """
        return self._set_load_switch(int(state))


    def get_load_switch(self):
        """Gets the load switch.
        """
        status, state = self._get_load_switch()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_switch(self, state):
        """Sets the output switch.
        """
        return self._set_output_switch(int(state))


    def get_output_switch(self):
        """Gets the output switch.
        """
        status, state = self._get_output_switch()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def get_meas_gains(self):
        """Gets conversion gains.
        """
        status, gains = self._get_meas_gains()
        if status != 0:
            return (-1, status)
        
        return (status, gains)


    def clear_status(self):
        """Clears the hardware status.
        """
        return self._clear_status()

    
    def get_status(self):
        """Gets hardware status
        """
        status, hw_status = self._get_status()
        if status != 0:
            return (-1, status)
        
        return (status, hw_status)
    
    def _set_pwm_reset(self, reset):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_reset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(reset, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM reset. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_reset(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_reset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, reset = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM reset. Error code {:}\r\n'.format(status))
            return (-1, status)

        reset = lrssoc.conversions.u8_to_u32(reset, msb=False)
        
        return (0, reset)
    

    def _set_pwm_output_enable(self, enable):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_output_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(enable, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM output enable. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_output_enable(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_output_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM output enable. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = lrssoc.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)


    def _set_pwm_ovf_trigger_enable(self, enable):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_ovf_trigger_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(enable, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM ovf trigger. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_ovf_trigger_enable(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_ovf_trigger_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM ovf trigger. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = lrssoc.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)
    

    def _set_pwm_freq(self, freq):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM frequency. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_freq(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = lrssoc.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)

        
    def _set_pwm_duty(self, duty):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_pwm_duty
        
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', duty)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM duty. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_duty(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_pwm_duty
        
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, duty_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM duty. Error code {:}\r\n'.format(status))
            return (-1, status)

        duty = struct.unpack('<f', duty_b)[0]
        
        return (0, duty)
    
    
    def _set_pwm_dead_time(self, dead_time):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_pwm_dead_time

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', dead_time)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM dead time. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_dead_time(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_pwm_dead_time

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, dt_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM dead time. Error code {:}\r\n'.format(status))
            return (-1, status)

        dt = struct.unpack('<f', dt_b)[0]
        
        return (0, dt)

    
    def _set_adc_enable(self, enable):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_adc_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(enable, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting ADC enable. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_adc_enable(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_adc_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC enable. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = lrssoc.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)
    
    
    def _set_adc_manual_trigger(self, trigger):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_adc_manual_trigger

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(trigger, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting ADC manual trigger. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_adc_manual_trigger(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_adc_manual_trigger

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, trigger = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC manual trigger. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        trigger = lrssoc.conversions.u8_to_u32(trigger, msb=False)
        
        return (0, trigger)

    
    def _set_adc_int_enable(self, enable):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_adc_int_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(enable, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting ADC int enable. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)
    

    def _get_adc_int_enable(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_adc_int_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC int enable. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        enable = lrssoc.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)
    

    def _set_adc_spi_freq(self, freq):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_adc_spi_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(freq, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting ADC SPI frequency. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_adc_spi_freq(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_adc_spi_freq

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC SPI frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = lrssoc.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)


    def _set_load_switch(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_load_switch

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the load switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_load_switch(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_load_switch

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the load switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = lrssoc.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_output_switch(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_switch

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_switch(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_switch

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = lrssoc.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _get_meas_gains(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_meas_gains

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, gains = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting meas gains. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0, gains)


    def _clear_status(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.clear_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error clearing the hardware status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)

    
    def _get_status(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        status, hw_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting hardware status. Error code {:}\r\n'.format(status))
            return (-1, status)

        hw_status = lrssoc.conversions.u8_to_u32(hw_status, msb=False)
        
        return (0, hw_status)
