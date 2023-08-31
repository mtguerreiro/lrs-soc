##"""
##Module ``buck``
##==============
##
##
##"""
##import lrssoc
##
##class Commands:
##    """
##    """
##    def __init__(self):
##        self.blink = 0
##        self.adc_en = 1
##        self.adc_config = 2
##        self.pwm_config = 3
##        
##class Buck:
##    """
##
##    Parameters
##    ----------
##
##    Raises
##    ------
##
##    Attributes
##    ----------
##        
##    """
##
##
##    def adc_en(self, en):
##        """Enables/disables the ADC.
##
##        Parameters
##        ----------
##        en : bool
##            If `True`, enables ADC. If `False`, disables ADC.
##
##        Raises
##        ------
##        TypeError
##            If `en` is not of `bool` type.
##
##        """        
##        if type(en) is not bool:
##            raise TypeError('`en` must be of bool type.')
##        
##        cmd = self.cmd.adc_en
##
##        if en == True:
##            en = [1]
##        else:
##            en = [0]
##
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
##        tx_data.extend( lrssoc.conversions.u32_to_u8(en, msb=False) )
##
##        status, _ = self.hwi.cs_hardware_if(0, tx_data)
##
##        if status < 0:
##            funcname = AFE.adc_en.__name__
##            print('{:}: Error enabling ADC. Error code {:}\r\n'.format(funcname, status))
##            
##        return status
##
##    
##    def adc_config(self, spifreq=10):
##        """Sets the ADC's SPI clock frequency.
##
##        The clock will be divided by 2 x `freq`.
##
##        Parameters
##        ----------
##        spifreq : int
##            Clock division factor. By default, it is 10.
##
##        Raises
##        ------
##        TypeError
##            If `freq` is not of `int` type.
##
##        """        
##        cmd = self.cmd.adc_config
##
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
##        tx_data.extend( lrssoc.conversions.u32_to_u8(spifreq, msb=False) )
##
##        status, _ = self.hwi.cs_hardware_if(0, tx_data)
##
##        if status < 0:
##            funcname = AFE.adc_config.__name__
##            print('{:}: Error configuring ADC. Error code {:}\r\n'.format(funcname, status))
##            
##        return status
##    
##
##    def pwm_config(self, pwmfreq=10000):
##        """Sets the PWM frequency. This is also the ADC's sampling frqeuency.
##
##        The frequency will be given by MAIN_CLOCK / (2 x `freq`).
##
##        Parameters
##        ----------
##        pwmfreq : int
##            Clock division factor. By default, it is 10000.
##
##        Raises
##        ------
##        TypeError
##            If `freq` is not of `int` type.
##
##        """        
##        cmd = self.cmd.pwm_config
##
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
##        tx_data.extend( lrssoc.conversions.u32_to_u8(pwmfreq, msb=False) )
##
##        status, _ = self.hwi.cs_hardware_if(0, tx_data)
##
##        if status < 0:
##            funcname = AFE.pwm_config.__name__
##            print('{:}: Error configuring PWM. Error code {:}\r\n'.format(funcname, status))
##
##        return status
##
##
##    def set_controller(self, controller=0):
##        """Sets the controller.
##
##        Parameters
##        ----------
##        controller : int
##            Controller.
##
##        Raises
##        ------
##        TypeError
##            If `freq` is not of `int` type.
##
##        """
##        tx_data = []
##        tx_data.extend( lrssoc.conversions.u32_to_u8(0, msb=False) )
##        tx_data.extend( lrssoc.conversions.u32_to_u8(controller, msb=False) )
##
##        status, _ = self.hwi.cs_controller_if(0, tx_data)
##
##        if status < 0:
##            funcname = AFE.set_controller.__name__
##            print('{:}: Error setting the controller. Error code {:}\r\n'.format(funcname, status))
##            
##        return status
##
