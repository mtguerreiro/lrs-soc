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
        self.set_pwm_output_enable = 1
        self.set_pwm_ovf_trigger_enable = 2
        self.set_pwm_freq = 3
        self.set_pwm_duty = 4
        self.set_pwm_dead_time = 5
        self.set_adc_enable = 6
        self.set_adc_manual_trigger = 7
        self.set_adc_int_enable = 8
        self.set_adc_spi_freq = 9
        
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


    def pwm_reset(self,reset):

        return self._set_pwm_reset(reset)


    def pwm_output_enable(self, enable):

        return self._set_pwm_output_enable(enable)


    def pwm_ovf_trigger_enable(self, enable):

        return self._set_pwm_ovf_trigger_enable(enable)


    def pwm_set_frequency(self, freq):
        """Sets PWM frequency, in kHz.
        """
        return self._set_pwm_freq(int(freq * 1000))


    def pwm_set_duty(self, duty):
        """Sets PWM duty, in percentage.
        """
        return self._set_pwm_duty(float(duty))


    def pwm_set_dead_time(self, dead_time):
        """Sets PWM dead time, in ns.
        """
        return self._set_pwm_dead_time(float(dead_time * 1e-9))
    

    def adc_enable(self, enable):
        """Enables the ADC.
        """

        return self._set_adc_enable(enable)


    def adc_trigger(self):
        """Triggers the ADC for a single conversion.
        """

        status = self._set_adc_manual_trigger(0)
        if status[0] != 0:
            return (status, err)
        
        status = self._set_adc_manual_trigger(1)
        if status[0] != 0:
            return (status, err)
        
        return self._set_adc_manual_trigger(0)
    

    def adc_int_enable(self, enable):
        """Enables ADC interrupt.
        """

        return self._set_adc_int_enable(enable)

    
    def adc_set_spi_freq(self, freq):
        """Sets ADC SPI clock frequency, in kHz.
        """
        return self._set_adc_spi_freq(int(freq * 1000))

    
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
