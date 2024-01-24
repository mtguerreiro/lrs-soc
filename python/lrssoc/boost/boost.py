"""
Module ``boost``
================


"""
import lrssoc
import numpy as np
import matplotlib.pyplot as plt


class Boost:
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

        self._ctl_if = lrssoc.boost.boost_controller.Controller(ocp_if=self._ocp_if, cs_id=cs_id)

        self._hw_if = lrssoc.boost.boost_hw.Hw(ocp_if=self._ocp_if, cs_id=cs_id)

        self._tr_if = lrssoc.boost.boost_trace.Trace(ocp_if=self._ocp_if, tr_id=tr_id)
        self._tr_id = tr_id

        self._plot = lrssoc.boost.boost_plot.Plot()

    
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


    def set_input_relay(self, state):

        if state != 0: state = 1
        
        return self._hw_if.set_input_relay( int(state) )


    def set_output_relay(self, state):

        if state != 0: state = 1
        
        return self._hw_if.set_output_relay( int(state) )    
 
    # ========================================================================

    # ========================================================================
    # ============================ Plot functions ============================
    # ========================================================================

    def plot(self, data, t=None, ax=None):
        self._plot.measurements(data, t)
        

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
