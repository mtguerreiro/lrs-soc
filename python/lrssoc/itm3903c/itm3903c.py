"""
Module ``itm3903c``
===================


"""
import lrssoc
import numpy as np
import matplotlib.pyplot as plt
import json


class ITM3903C:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, comm, comm_settings, cs_id = 0, tr_id=0):

        self._cs_id = cs_id

        self._ocp_if = lrssoc.ocp.iface.Interface(comm_type=comm, settings=comm_settings)

        self._ctl_if = lrssoc.itm3903c.itm3903c_controller.Controller(ocp_if=self._ocp_if, cs_id=cs_id)

        self._hw_if = lrssoc.itm3903c.itm3903c_hw.DigitalHw(ocp_if=self._ocp_if, cs_id=1)

        self._an_hw_if = lrssoc.itm3903c.itm3903c_hw.AnalogHw(ocp_if=self._ocp_if, cs_id=0)

        self._tr_if = lrssoc.itm3903c.itm3903c_trace.Trace(ocp_if=self._ocp_if, tr_id=tr_id)
        self._tr_id = tr_id

        #self._plot = lrssoc.cuk.cuk_plot.Plot()

    
    # ========================================================================
    # =========================== System functions ===========================
    # ========================================================================
    
    def initSupply(self, settings):

        if settings["status"] == "0":
            print("baord not active \n")
            
        if settings["status"] == "1":
            print("board calibrated \n")
            dac = self._an_hw_if.set_dac_cal_data({   
                'dac1_adj_gain': settings["DACADJ"]["gain"],
                'dac1_adj_offset': settings["DACADJ"]["Offset"],
                'dac1_offset_gain': settings["DACOFFSET"]["gain"],
                'dac1_offset_offset':settings["DACOFFSET"]["Offset"],
                'dac2_gain': settings["DAC2"]["gain"],
                'dac2_offset': settings["DAC2"]["Offset"],
                'dac3_gain': settings["DAC3"]["gain"],
                'dac3_offset':settings["DAC3"]["Offset"]
            })

            if dac[0] >= 0:
                print("DAC calibration data set\n")
            else:
                print("error setting DAC calibration data\n")
                
            adc = self._an_hw_if.set_adc_cal_data({   
                'v_gain': settings["ADCV"]["gain"],
                'v_ofs': settings["ADCV"]["Offset"],
                'i_gain': settings["ADCC"]["gain"],
                'i_ofs': settings["ADCC"]["Offset"],
            })

            if adc[0] >= 0:
                print("ADC calibration data set\n")
            else:
                print("error setting ADC calibration data\n")

            alpha = self._an_hw_if.set_alpha_values({   
                'v': settings["AValues"]["v"],
                'i': settings["AValues"]["i"],
            })
          
            if alpha[0] >= 0:
                print("Alpha values set\n")
            else:
                print("error setting alpha values\n")

            #Function Mode
            if 'mode' not in settings:
                print('Cannot run setup if `mode` is not specified.\n\r')
                return (-1, )

            if (settings['mode'] != 'current') and (settings['mode'] != 'voltage'):
                print('Cannot run setup. `mode` must be \'voltage\' or \'current\'')
                return (-1,)
            
            print('Setting mode to {:}...'.format(settings['mode']))
            status = self._hw_if.set_func_mode(settings['mode'])
            if( status[0] < 0 ):
                print('Could not complete setup. Error setting `mode` ({:})'.format(status[1]))
                return (-1, status[1])
            else:
                print('Mode set.\n\r')

            #Power Status
            powstat = self._hw_if.set_output_status(settings["powstatus"])
            if (powstat[0] < 0):
                print("Error setting output status")
            elif (settings["powstatus"] != 0) and (settings["powstatus"] != 1):
                print("Output status must be either True or False")

            #Analog Status
            anstat = self._hw_if.set_analog_external_status(settings['analog external status'])
            if (anstat[0]) < 0:
                print("error setting external analog status ")
            elif(settings['analog external status'] != True) and (settings['analog external status'] != False):
                print("Analog external status must be set to True or False")
            
            #Power Limit
            if (settings['mode'] == "voltage"):
                plim = settings["Voltage Supply Settings"]['power_lim']
                print('Setting power limits to {:} W and {:} W...'.format(plim[0], plim[1]))
                status_min = self._hw_if.set_power_min(plim[0])
                status_max = self._hw_if.set_power_max(plim[1])
                if( (status_max[0] < 0) or (status_min[0] < 0) ):
                    print('Error setting power limits... Status: {:} {:}.\n\r'. format(status_min[1], status_max[1]))
                    return ( -1, (status_min[1], status_max[1]) )
                else:
                    print('Power limits set.\n\r')

            elif (settings['mode'] == "current"):
                plim = settings["Current Supply Settings"]['power_lim']
                print('Setting power limits to {:} W and {:} W...'.format(plim[0], plim[1]))
                status_min = self._hw_if.set_power_min(plim[0])
                status_max = self._hw_if.set_power_max(plim[1])
                if( (status_max[0] < 0) or (status_min[0] < 0) ):
                    print('Error setting power limits... Status: {:} {:}.\n\r'. format(status_min[1], status_max[1]))
                    return ( -1, (status_min[1], status_max[1]) )
                else:
                    print('Power limits set.\n\r')

            else:
                print('Skipping setting power limits...\n\r')

            #Current Limits
            if (settings['mode'] == 'voltage'):
                ilim = settings["Voltage Supply Settings"]['curr_lim']
                print('Setting current limits to {:} A and {:} A...'.format(ilim[0], ilim[1]))
                status_min = self._hw_if.set_current_min(ilim[0])
                status_max = self._hw_if.set_current_max(ilim[1])
                if( (status_max[0] < 0) or (status_min[0] < 0) ):
                    print('Error setting current limits... Status: {:} {:}.\n\r'. format(status_min[1], status_max[1]))
                    return ( -1, (status_min[1], status_max[1]) )
                else:
                    print('Current limits set.\n\r')

            elif ( (settings['mode'] == 'voltage')):
                print('Skipping setting current limits...\n\r')

            #Voltage Limits
            if (settings['mode'] == 'current' ):
                vlim = settings["Current Supply Settings"]['volt_lim']
                print('Setting voltage limits to {:} V and {:} V...'.format(vlim[0], vlim[1]))
                status_min = self._hw_if.set_voltage_min(vlim[0])
                status_max = self._hw_if.set_voltage_max(vlim[1])
                if( (status_max[0] < 0) or (status_min[0] < 0) ):
                    print('Error setting voltage limits... Status: {:} {:}.\n\r'. format(status_min[1], status_max[1]))
                    return ( -1, (status_min[1], status_max[1]) )
                else:
                    print('Voltage limits set.\n\r')

            elif ( settings['mode'] == 'current' ):
                print('Skipping setting voltage limits...\n\r')
            
            #Set Analog 1    
            if settings["mode"] == "voltage":
                ain, aout = settings["Voltage Supply Settings"]['analog_1'][0], settings["Voltage Supply Settings"]['analog_1'][1]
                print('Setting analog channel 1: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(1, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 1... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 1 set.\n\r')

            elif settings["mode"] == "current":
                ain, aout = settings["Current Supply Settings"]['analog_1'][0], settings["Current Supply Settings"]['analog_1'][1]
                print('Setting analog channel 1: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(1, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 1... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 1 set.\n\r')
            else:
                print('Skipping setting analog channel 1.\n\r')


            #Set Analog 2
            if settings["mode"] == "voltage":
                ain, aout = settings["Voltage Supply Settings"]['analog_2'][0], settings["Voltage Supply Settings"]['analog_2'][1]
                print('Setting analog channel 2: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(2, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 2... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 2 set.\n\r')
            elif settings["mode"] == "current":
                ain, aout = settings["Current Supply Settings"]['analog_2'][0], settings["Current Supply Settings"]['analog_2'][1]
                print('Setting analog channel 2: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(2, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 2... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 2 set.\n\r')
            else:
                print('Skipping setting analog channel 2.\n\r')

            #Set Analog 3
            if settings["mode"] == "voltage":
                ain, aout = settings["Voltage Supply Settings"]['analog_3'][0], settings["Voltage Supply Settings"]['analog_3'][1]
                print('Setting analog channel 3: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(3, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 3... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 3 set.\n\r')
            elif settings["mode"] == "current":
                ain, aout = settings["Current Supply Settings"]['analog_3'][0], settings["Current Supply Settings"]['analog_3'][1]
                print('Setting analog channel 3: {:}\t{:}...'.format(ain, aout))
                status = self._hw_if.config_slope_offset(3, ain, aout)
                if( status[0] < 0 ):
                    print('Error setting analog channel 3... Status: {:}\n\r'.format(status[1]))
                else:
                    print('Analog channel 3 set.\n\r')
            else:
                print('Skipping setting analog channel 3.\n\r')
                
            print('Setup completed.\n\r')
            

  
    

    def enable(self):
        """
        """
        status, = self._ocp_if.cs_enable( self._cs_id )
        if status != 0:
            return (-1, status)

        return (0,)
        

    def disable(self):
        """
        """
        status, = self._ocp_if.cs_disable( self._cs_id )
        if status != 0:
            return (-1, status)
        
        return (0,)


    def status(self):
        """
        """
        cmdstatus, status = self._ocp_if.cs_status( self._cs_id )
        if cmdstatus != 0 :
            print('Error getting status')
            return (-1, cmd_status)

        return (0, status)

    # ========================================================================

    # ========================================================================
    # ========================= Controller functions =========================
    # ========================================================================
    
    def disable_controller(self):
        """
        """
        status, = self._ctl_if.set( 0 )
        if status != 0:
            return (-1, status)

        return (0,)

    
    def enable_controller(self, controller, reset=True):
        """
        """
        if reset is True:
            status, = self._ctl_if.reset( controller )
            if status < 0:
                return (-1, status)
        
        status, = self._ctl_if.set( controller )
        if status < 0:
            return (-1, status)      

        return (0,)


    def set_controller_params(self, controller, params):
        """
        """
        status, new_params = self._ctl_if.get_params( controller )
        if status < 0:
            return (-1, status)

        for param, val in params.items():
            if param in new_params:
                new_params[param] = val
                
        status, = self._ctl_if.set_params( controller, new_params )
        if status < 0:
            return (-1, status)      

        return (0,)


    def get_controller(self):
        """
        """
        status, controller = self._ctl_if.get()
        if status != 0:
            return (-1, status)

        return (0, controller)


    def get_controller_params(self, controller):
        """
        """
        status, params = self._ctl_if.get_params(controller)
        if status != 0:
            return (-1, status)

        return (0, params)


    def set_ref(self, ref):
        """
        """
        status = self._ctl_if.set_ref(ref)
        if status[0] != 0:
            return (-1, status)

        return (0,)        


    def get_ref(self):
        """
        """
        status, ref = self._ctl_if.get_ref()
        if status != 0:
            return (-1, status)

        return (0, ref)

    # ------------------------------------------------------------------------
    # ------------------------ Open-loop controller --------------------------
    # ------------------------------------------------------------------------
    def ol_ctl_enable(self, reset=False):

        return self.enable_controller('ol', reset=reset)
    

##    def startup_ctl_set_params(self, uinc=None, ufinal=None):
##
##        params = {}
##        if uinc is not None:
##            params['uinc'] = float(uinc)
##        if ufinal is not None:
##            params['ufinal'] = float(ufinal)
##
##        return self.set_controller_params('startup', params)
##
##
##    def startup_ctl_get_params(self):
##
##        return self.get_controller_params('startup')
        
    # ------------------------------------------------------------------------
    
    # ========================================================================
    
    # ========================================================================
    # =========================== Trace functions ============================
    # ========================================================================
    def read_trace(self):
        """
        """
        status, (traces, trace_data) = self._tr_if.read()
        if status != 0:
            return (-1, status)

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        t = 1 / freq * np.arange( len(trace_data[0]) )
        trace_data = np.array(trace_data).T
        
        return (0, (traces, trace_data, t))


    def reset_trace(self):
        """
        """
        status, = self._tr_if.reset()
        if status != 0:
            return (-1, status)

        return (0,)


    def set_trace_size(self, size):
        """
        """
        status = self._tr_if.set_size(size)
        if status[0] != 0:
            return (-1, status[0])

        return (0,)


    def get_trace_size(self):
        """
        """
        status, size = self._tr_if.get_size()
        if status != 0:
            return (-1, status)

        return (0, size)

    # ========================================================================
    
    # ========================================================================
    # ============================= HW functions =============================
    # ========================================================================
    
    def get_hw_status(self):

        status, hw_status = self._hw_if.get_status()
        if status != 0:
            return (-1, status)

        return (0, hw_status)
    

    def clear_hw_status(self):

        status = self.disable_controller()

        if status[0] != 0:
            return (-1, status)

        return self._hw_if.clear_status()
        
    # ========================================================================

