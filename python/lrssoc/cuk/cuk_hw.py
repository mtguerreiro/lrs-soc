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
        self.set_pwm_period = 3
        self.set_pwm_duty = 4
        self.set_pwm_dead_time = 5
        
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

        self._period = 500


    def pwm_reset(self,reset):

        return self._set_pwm_reset(reset)


    def pwm_output_enable(self, enable):

        return self._set_pwm_output_enable(enable)


    def pwm_ovf_trigger_enable(self, enable):

        return self._set_pwm_ovf_trigger_enable(enable)


    def pwm_set_frequency(self, frequency):
        """Sets PWM frequency, in kHz.
        """

        period_int = int( 100e6 / (2 * frequency * 1e3) )
        self._period = period_int
        
        return self._set_pwm_period(period_int)


    def pwm_set_duty(self, duty):
        """Sets PWM duty, in percentage.
        """

        duty_int = int(duty / 100 * self._period)

        return self._set_pwm_duty(duty_int)


    def pwm_set_dead_time(self, dead_time):
        """Sets PWM dead time, in ns.
        """

        dead_time_int = int(dead_time * 1e-9 * 100e6)

        return self._set_pwm_dead_time(dead_time_int)
    

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


    def _set_pwm_period(self, period):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_period

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(period, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM period. Error code {:}\r\n'.format(status))
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
        tx_data.extend( lrssoc.conversions.u32_to_u8(duty, msb=False) )
        
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
        tx_data.extend( lrssoc.conversions.u32_to_u8(dead_time, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM dead time. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)
