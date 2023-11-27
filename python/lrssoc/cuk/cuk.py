"""
Module ``cuk``
==============


"""
import lrssoc
import numpy as np
import matplotlib.pyplot as plt

class Commands:
    """
    """
    def __init__(self):
        self.blink = 0
        self.adc_en = 1
        self.adc_config = 2
        self.pwm_config = 3


class Cuk:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, cs_id, comm, comm_settings, tr_id=0):

        self._cs_id = 0

        self._ocp_if = lrssoc.ocp.iface.Interface(comm_type=comm, settings=comm_settings)

        self._ctl_if = lrssoc.cuk.cuk_controller.Controller(ocp_if=self._ocp_if, cs_id=cs_id)

        self._hw_if = lrssoc.cuk.cuk_hw.Hw(ocp_if=self._ocp_if, cs_id=cs_id)

        self._tr_if = lrssoc.cuk.cuk_trace.Trace(ocp_if=self._ocp_if, tr_id=tr_id)
        self._tr_id = tr_id

        self._plot = lrssoc.cuk.cuk_plot.Plot()

    
    # ========================================================================
    # =========================== System functions ===========================
    # ========================================================================
    
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
    # -------------------------- Startup controller --------------------------
    # ------------------------------------------------------------------------
    def startup_ctl_enable(self, reset=False):

        return self.enable_controller('startup', reset=reset)
    

    def startup_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if uinc is not None:
            params['uinc'] = float(uinc)
        if ufinal is not None:
            params['ufinal'] = float(ufinal)

        return self.set_controller_params('startup', params)


    def startup_ctl_get_params(self):

        return self.get_controller_params('startup')
        
    # ------------------------------------------------------------------------
    
    # ------------------------------------------------------------------------
    # --------------------------- Energy controller --------------------------
    # ------------------------------------------------------------------------
    def energy_ctl_enable(self, reset=True):

        return self.enable_controller('energy', reset=reset)
        
        
    def energy_ctl_set_time_resp(self, ts, os):

        ec = lrssoc.cuk.cuk_controller.Energy()

        gains = ec.gains(float(ts), float(os))

        return self.set_controller_params('energy', gains)


    def energy_ctl_get_time_resp(self):

        ec = lrssoc.cuk.cuk_controller.Energy()
        
        status, params = self.get_controller_params('energy')
        if status != 0:
            return (-1, status)

        return params['k1'], params['k2']

        #return ec.time_resp_from_gains(params['k1'], params['k2'])


    def energy_ctl_set_notch(self, fc, Q):

        ec = lrssoc.cuk.cuk_controller.Energy()

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        dt = 1 / float(freq)

        filt = ec.discrete_notch(float(fc), float(Q), dt)

        return self.set_controller_params('energy', filt)


    def energy_ctl_get_notch(self):

        status, params = self.get_controller_params('energy')
        if status != 0:
            return (-1, status)

        filt = {
            'a0':params['a0'], 'a1':params['a1'], 'a2':params['a2'],
            'b1':params['b1'], 'b2':params['b2'],
            'notch_en':params['notch_en']
            }
        
        return filt


    def energy_ctl_enable_notch(self):

        en = {'notch_en':1.0}
        
        return self.set_controller_params('energy', en)


    def energy_ctl_disable_notch(self):

        en = {'notch_en':0.0}
        
        return self.set_controller_params('energy', en)


    def energy_ctl_status_notch(self):

        status, params = self.get_controller_params('energy')
        if status != 0:
            return (-1, status)

        en = params['notch_en']

        return (0, en)
    
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    # ------------------------- EnergyInt controller -------------------------
    # ------------------------------------------------------------------------
    def energy_int_ctl_enable(self, reset=True):

        return self.enable_controller('energy_int', reset=reset)
        
        
    def energy_int_ctl_set_time_resp(self, ts, os, alpha=5.0):

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        dt = float(1) / float(freq)
        
        ec = lrssoc.cuk.cuk_controller.EnergyInt()

        gains = ec.gains(float(ts), float(os), dt=dt, alpha=alpha)

        return self.set_controller_params('energy_int', gains)


    def energy_int_ctl_set_notch(self, fc, Q):

        ec = lrssoc.cuk.cuk_controller.EnergyInt()

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        dt = 1 / float(freq)

        filt = ec.discrete_notch(float(fc), float(Q), dt)

        return self.set_controller_params('energy_int', filt)


    def energy_int_ctl_get_notch(self):

        status, params = self.get_controller_params('energy_int')
        if status != 0:
            return (-1, status)

        filt = {
            'a0':params['a0'], 'a1':params['a1'], 'a2':params['a2'],
            'b1':params['b1'], 'b2':params['b2'],
            'notch_en':params['notch_en']
            }
        
        return filt


    def energy_int_ctl_enable_notch(self):

        en = {'notch_en':1.0}
        
        return self.set_controller_params('energy_int', en)


    def energy_int_ctl_disable_notch(self):

        en = {'notch_en':0.0}
        
        return self.set_controller_params('energy_int', en)


    def energy_int_ctl_status_notch(self):

        status, params = self.get_controller_params('energy_int')
        if status != 0:
            return (-1, status)

        en = params['notch_en']

        return (0, en)
    
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    # ---------------------------- State feedback ----------------------------
    # ------------------------------------------------------------------------
    def sfb_ctl_enable(self, reset=True):

        return self.enable_controller('sfb', reset=reset)
        
        
    def sfb_ctl_set_time_resp(self, ts):

        sfbc = lrssoc.cuk.cuk_controller.SFB()

        params = sfbc.params(float(ts))

        return self.set_controller_params('sfb', params)


    def sfb_ctl_get_time_resp(self):

        #sfbc = lrssoc.cuk.cuk_controller.SFB()
        
        status, params = self.get_controller_params('sfb')
        if status != 0:
            return (-1, status)

        return params
    
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    # ------------------------- State feedback + int -------------------------
    # ------------------------------------------------------------------------
    def sfb_int_ctl_enable(self, reset=True):

        return self.enable_controller('sfb_int', reset=reset)
        
        
    def sfb_int_ctl_set_time_resp(self, ts, os):

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        dt = 1 / float(freq)
        
        sfbc = lrssoc.cuk.cuk_controller.SFBINT()

        params = sfbc.params(float(ts), float(os), dt=dt)

        return self.set_controller_params('sfb_int', params)


    def sfb_int_ctl_get_time_resp(self):

        #sfbc = lrssoc.cuk.cuk_controller.SFBINT()
        
        status, params = self.get_controller_params('sfb_int')
        if status != 0:
            return (-1, status)

        return params
    
    # ------------------------------------------------------------------------


    # ------------------------------------------------------------------------
    # --------------------------------- PCH ----------------------------------
    # ------------------------------------------------------------------------
    def pch_ctl_enable(self, reset=True):

        return self.enable_controller('pch', reset=reset)
        
        
    def pch_ctl_set_gains(self, kj2=4405286.343612335, kj3=-5988023.952095808, kr2=5286343.612334802):

        pchc = lrssoc.cuk.cuk_controller.PCH()

        params = pchc.params(float(kj2), float(kj3), float(kr2))

        return self.set_controller_params('pch', params)


    def pch_ctl_get_gains(self):
        
        status, params = self.get_controller_params('pch')
        if status != 0:
            return (-1, status)

        return params
    
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

        return self._hw_if.clear_status()


    def set_load_switch(self, state):

        if state != 0: state = 1
        
        return self._hw_if.set_load_switch( int(state) )
    

    def set_low_pass_coef(self, alpha):
        """Sets low-pass filter coef (0 < alpha < 1).
        """
        return self._hw_if.set_low_pass_filt_coef(alpha)

        
    def get_low_pass_coef(self):
        """Gets low-pass filter coef.
        """
        return self._hw_if.get_low_pass_filt_coef()
        
    # ========================================================================

    # ========================================================================
    # ============================ Plot functions ============================
    # ========================================================================

    def plot(self, data, t=None, ax=None):
        self._plot.measurements(data, t)
        

    def plot_energy(self, data, t=None, ax=None):
        self._plot.energy(data, t)
        

    def plot_live(self, dt):

        status, freq = self._hw_if.get_pwm_frequency()
        if status != 0:
            return (-1, status)

        n_samples = round(dt * freq)

        self.set_trace_size(n_samples)

        t = 1 / freq * np.arange( n_samples )

        fig, axes = plt.subplots(nrows=2, ncols=2)
        fig.set_size_inches(10, 6)

        while True:
            status, (traces, data, t) = self.read_trace()
            self._plot.measurements(data, t, fig=fig)
            plt.pause(dt)
            self.reset_trace()
            
    # ========================================================================
