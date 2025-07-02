"""
Module ``fsbuckboost_hw``
=========================


"""
import pyocp
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

        self.set_pwm_inv = 6
        self.get_pwm_inv = 7

        self.set_pwm_mode = 8
        self.get_pwm_mode = 9

        self.set_pwm_ls_sw = 10
        self.get_pwm_ls_sw = 11

        self.set_pwm_hs_sw = 12
        self.get_pwm_hs_sw = 13
        
        self.set_pwm_freq = 14
        self.get_pwm_freq = 15
        
        self.set_pwm_duty = 16
        self.get_pwm_duty = 17
        
        self.set_pwm_dead_time = 18
        self.get_pwm_dead_time = 19
        
        self.set_adc_enable = 20
        self.get_adc_enable = 21
        
        self.set_adc_manual_trigger = 22
        self.get_adc_manual_trigger = 23
        
        self.set_adc_int_enable = 24
        self.get_adc_int_enable = 25
        
        self.set_adc_spi_freq = 26
        self.get_adc_spi_freq = 27

        self.set_input_prech = 28
        self.get_input_prech = 29

        self.set_input_disch = 30
        self.get_input_disch = 31

        self.set_input_ssr_p = 32
        self.get_input_ssr_p = 33

        self.set_input_ssr_n = 34
        self.get_input_ssr_n = 35

        self.set_output_prech = 36
        self.get_output_prech = 37

        self.set_output_disch = 38
        self.get_output_disch = 39

        self.set_output_ssr_p = 40
        self.get_output_ssr_p = 41

        self.set_output_ssr_n = 42
        self.get_output_ssr_n = 43
        
        self.clear_status = 44
        self.get_status = 45


class MeasGains:

    def __init__(self):

        self.keys = (
            'ii_gain', 'ii_ofs', 'il_gain', 'il_ofs', 'io_gain', 'io_ofs',
            'vi_gain', 'vi_ofs', 'vo_dc_gain', 'vo_dc_ofs', 'vo_gain', 'vo_ofs'
        )

    def encode(self, gains):

        keys = self.keys
        
        _gains = [gains[key] for key in keys]
        gains_bin = struct.pack(f'<{len(keys)}f', *_gains)

        return gains_bin

    
    def decode(self, data_bin):

        keys = self.keys
        
        data = struct.unpack(f'<{len(keys)}f', data_bin)
        gains = dict(zip(keys, data))

        return gains

        
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
        self._meas_gains = MeasGains()


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


    def set_pwm_inv(self, enable):

        return self._set_pwm_inv(enable)


    def get_pwm_inv(self):

        return self._get_pwm_inv()

    
    def set_pwm_mode(self, mode):

        return self._set_pwm_mode(mode)


    def get_pwm_mode(self):

        return self._get_pwm_mode()


    def set_pwm_ls_sw(self, state):

        return self._set_pwm_ls_sw(state)


    def get_pwm_ls_sw(self):

        return self._get_pwm_ls_sw()


    def set_pwm_hs_sw(self, state):

        return self._set_pwm_hs_sw(state)


    def get_pwm_hs_sw(self):

        return self._get_pwm_hs_sw()

    
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


    def set_input_prech(self, state):

        return self._set_input_prech(int(state))


    def get_input_prech(self):
        status, state = self._get_input_prech()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_input_disch(self, state):

        return self._set_input_disch(int(state))


    def get_input_disch(self):
        status, state = self._get_input_disch()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_input_ssr_p(self, state):

        return self._set_input_ssr_p(int(state))


    def get_input_ssr_p(self):
        status, state = self._get_input_ssr_p()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_input_ssr_n(self, state):

        return self._set_input_ssr_n(int(state))


    def get_input_ssr_n(self):
        status, state = self._get_input_ssr_n()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_prech(self, state):

        return self._set_output_prech(int(state))


    def get_output_prech(self):
        status, state = self._get_output_prech()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_disch(self, state):

        return self._set_output_disch(int(state))


    def get_output_disch(self):
        status, state = self._get_output_disch()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_ssr_p(self, state):

        return self._set_output_ssr_p(int(state))


    def get_output_ssr_p(self):
        status, state = self._get_output_ssr_p()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_ssr_n(self, state):

        return self._set_output_ssr_n(int(state))


    def get_output_ssr_n(self):
        status, state = self._get_output_ssr_n()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))
    
    
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(reset, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, reset = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM reset. Error code {:}\r\n'.format(status))
            return (-1, status)

        reset = pyocp.conversions.u8_to_u32(reset, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(enable, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM output enable. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = pyocp.conversions.u8_to_u32(enable, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(enable, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM ovf trigger. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = pyocp.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)
    

    def _set_pwm_inv(self, enable):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_inv

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(enable, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM inv. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_inv(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_inv

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM inv. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = pyocp.conversions.u8_to_u32(enable, msb=False)
        
        return (0, enable)


    def _set_pwm_mode(self, mode):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_mode

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(mode, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM mode. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_mode(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_mode

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, mode = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM mode. Error code {:}\r\n'.format(status))
            return (-1, status)

        mode = pyocp.conversions.u8_to_u32(mode, msb=False)
        
        return (0, mode)
    

    def _set_pwm_ls_sw(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_ls_sw

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM low-side switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_ls_sw(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_ls_sw

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting state of PWM low-side switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_pwm_hs_sw(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_hs_sw

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM high-side switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_hs_sw(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_hs_sw

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting state of PWM high-side switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)

    
    def _set_pwm_freq(self, freq):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_freq

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(freq, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = pyocp.conversions.u8_to_u32(freq, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(enable, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC enable. Error code {:}\r\n'.format(status))
            return (-1, status)

        enable = pyocp.conversions.u8_to_u32(enable, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(trigger, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, trigger = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC manual trigger. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        trigger = pyocp.conversions.u8_to_u32(trigger, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(enable, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, enable = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC int enable. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        enable = pyocp.conversions.u8_to_u32(enable, msb=False)
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(freq, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting ADC SPI frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = pyocp.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)


    def _set_input_prech(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_input_prech

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the input prech. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_input_prech(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_input_prech

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the input prech. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_input_disch(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_input_disch

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the input disch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_input_disch(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_input_disch

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the input disch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_input_ssr_p(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_input_ssr_p

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the input ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_input_ssr_p(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_input_ssr_p

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the input ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_input_ssr_n(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_input_ssr_n

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the input ssr n. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_input_ssr_n(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_input_ssr_n

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the input ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)
    

    def _set_output_prech(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_prech

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output prech. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_prech(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_prech

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output prech. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_output_disch(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_disch

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output disch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_disch(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_disch

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output disch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_output_ssr_p(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_ssr_p

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_ssr_p(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_ssr_p

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_output_ssr_n(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_ssr_n

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output ssr n. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_ssr_n(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_ssr_n

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output ssr p. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)

    
    def _clear_status(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.clear_status

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
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
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, hw_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting hardware status. Error code {:}\r\n'.format(status))
            return (-1, status)

        hw_status = pyocp.conversions.u8_to_u32(hw_status, msb=False)
        
        return (0, hw_status)
