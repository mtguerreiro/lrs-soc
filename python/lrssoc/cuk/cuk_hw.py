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
            print('Error setting ADC SPI frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = lrssoc.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)
