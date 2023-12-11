"""
Module ``mmc``
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


class MMC:
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

        self._tr_if = lrssoc.mmc.mmc_trace.Trace(ocp_if=self._ocp_if, tr_id=tr_id)
        self._tr_id = tr_id

        #self._plot = lrssoc.mmc.mmc_plot.Plot()

    
    # ========================================================================
    # =========================== System functions ===========================
    # ========================================================================
    
##    def enable(self):
##        """
##        """
##        status, = self._ocp_if.cs_enable( self._cs_id )
##        if status != 0:
##            return (-1, status)
##
##        return (0,)
##        
##
##    def disable(self):
##        """
##        """
##        status, = self._ocp_if.cs_disable( self._cs_id )
##        if status != 0:
##            return (-1, status)
##        
##        return (0,)
##
##
##    def status(self):
##        """
##        """
##        cmd_status, status = self._ocp_if.cs_status( self._cs_id )
##        if cmd_status != 0 :
##            print('Error getting status')
##            return (-1, cmd_status)
##
##        return (0, status)

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
    # ============================ Plot functions ============================
    # ========================================================================

##    def plot(self, data, t=None, ax=None):
##        self._plot.measurements(data, t)
##        
##
##    def plot_energy(self, data, t=None, ax=None):
##        self._plot.energy(data, t)
##        
##
##    def plot_live(self, dt):
##
##        status, freq = self._hw_if.get_pwm_frequency()
##        if status != 0:
##            return (-1, status)
##
##        n_samples = round(dt * freq)
##
##        self.set_trace_size(n_samples)
##
##        t = 1 / freq * np.arange( n_samples )
##
##        fig, axes = plt.subplots(nrows=2, ncols=2)
##        fig.set_size_inches(10, 6)
##
##        while True:
##            status, (traces, data, t) = self.read_trace()
##            self._plot.measurements(data, t, fig=fig)
##            plt.pause(dt)
##            self.reset_trace()
            
    # ========================================================================
