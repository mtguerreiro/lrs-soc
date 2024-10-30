"""
Module ``boost``
================


"""
import lrssoc
import numpy as np
import matplotlib.pyplot as plt
import time

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

        self._cs_id = cs_id

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

#added for new controller
    # ------------------------------------------------------------------------  
    # -------------------------- Energyc controller --------------------------
    # ------------------------------------------------------------------------
    def energyc_ctl_enable(self, reset=False):

        return self.enable_controller('energyc', reset=reset)
    

    def energyc_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if L is not None:
            params['L'] = float(L)
        if C is not None:
            params['C'] = float(C)
        if K1 is not None:
            params['K1'] = float(K1)
        if K2 is not None:
            params['K2'] = float(K2)

        return self.set_controller_params('energyc', params)


    def energyc_ctl_get_params(self):

        return self.get_controller_params('energyc')

    
 #added for new controller
    # ------------------------------------------------------------------------  
    # -------------------------- Energycint controller --------------------------
    # ------------------------------------------------------------------------
    def energycint_ctl_enable(self, reset=False):

        return self.enable_controller('energycint', reset=reset)
    

    def energycint_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if L is not None:
            params['L'] = float(L)
        if C is not None:
            params['C'] = float(C)
        if KI is not None:
            params['KI'] = float(KI)
        if K1 is not None:
            params['K1'] = float(K1)    
        if K2 is not None:
            params['K2'] = float(K2)
        if alpha is not None:
            params['alpha'] = float(alpha)

        return self.set_controller_params('energycint', params)


    def energycint_ctl_get_params(self):

        return self.get_controller_params('energycint')
    
 #added for new controller
    # ------------------------------------------------------------------------  
    # -------------------------- EnergycMPC controller --------------------------
    # ------------------------------------------------------------------------
    def energycmpc_ctl_enable(self, reset=False):

        return self.enable_controller('energycmpc', reset=reset)
    

    def energycmpc_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if L is not None:
            params['L'] = float(L)
        if C is not None:
            params['C'] = float(C)
        if d_min is not None:
            params['d_min'] = float(d_min)
        if d_max is not None:
            params['d_max'] = float(d_max)    
        if i_l_min is not None:
            params['i_l_min'] = float(i_l_min)
        if i_l_max is not None:
            params['i_l_max'] = float(i_l_max)

        return self.set_controller_params('energycmpc', params)


    def energycmpc_ctl_get_params(self):

        return self.get_controller_params('energycmpc')
    
    #-------------------------------------------------------------------------
     #added for new controller
    # ------------------------------------------------------------------------  
    # -------------------------- Linearization controller --------------------
    # This controller works with 2 frequencies and two processes:
    # - Process 1: Linearization part that converts rho to u (duty cycle) and is
    #  executed according to switching frequency SF (100 kHz).
    # - Process 2: Feedback linearization (Energycint) that produces rho and it is being executed 
    # according to control_frequency CF (SW/CF must be integer).
    # ------------------------------------------------------------------------
    def linearization_ctl_enable(self, reset=False):

        return self.enable_controller('linearization', reset=reset)
    

    def linearization_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if L is not None:
            params['L'] = float(L)
        if C is not None:
            params['C'] = float(C)
        if KI is not None:
            params['KI'] = float(KI)
        if K1 is not None:
            params['K1'] = float(K1)    
        if K2 is not None:
            params['K2'] = float(K2)
        if control_f is not None:
            params['control_f'] = float(control_f)
            
        return self.set_controller_params('linearization', params)


    def linearization_ctl_get_params(self):

        return self.get_controller_params('linearization')
    
  # ------------------------------------------------------------------------

   # ------------------------------------------------------------------------  
    # -------------------------- BF_MPC controller --------------------
    # This controller works with 2 frequencies and two processes:
    # - Process 1: Linearization part that converts rho to u (duty cycle) and is
    #  executed according to switching frequency SF (100 kHz).
    # - Process 2: MPC controller that produces rho and it is being executed 
    # according to control_frequency CF (SW/CF must be integer).
    # ------------------------------------------------------------------------
    def BF_MPC_ctl_enable(self, reset=False):

        return self.enable_controller('BF_MPC', reset=reset)
    

    def BF_MPC_ctl_set_params(self, uinc=None, ufinal=None):

        params = {}
        if L is not None:
            params['L'] = float(L)
        if C is not None:
            params['C'] = float(C)
        if KI is not None:
            params['i_l_min'] = float(i_l_min)
        if K1 is not None:
            params['i_l_max'] = float(i_l_max)    
        if control_f is not None:
            params['control_f'] = float(control_f)
        if alpha is not None:
            params['alpha'] = float(alpha)    
        if alpha_l is not None:
            params['alpha_l'] = float(alpha_l)
            
        return self.set_controller_params('BF_MPC', params)


    def BF_MPC_ctl_get_params(self):

        return self.get_controller_params('BF_MPC')
    
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

    #def plot_raw(self, data, t=None, ax=None):
     #   self._plot.measurements(data, t)   

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
    # ============================ Extra functions ============================
    # ========================================================================
 
   


        
    def plot_compare( a, file1, file2, vector1, vector2, vectorref1, vectorref2, valueref1, valueref2, op1, op2, t1, t2):
        # Load data from both files
        data1 = np.loadtxt(file1, delimiter=',', skiprows=1)
        data2 = np.loadtxt(file2, delimiter=',', skiprows=1)
        vref1 = data1[:, vectorref1]  # Column corresponding to reference vector1
        vref2 = data2[:, vectorref2]  # Column corresponding to reference vector2
        
        
        # Find indices where the value is equal to valueref for file1 in the column vectorref1
        i_1 = np.where(vref1 == valueref1)[0]
        if len(i_1) > 0:
            first1 = i_1[0]
            print("In file1, the first value equal to ", valueref1, " is at i: ", first1)
            new1 = data1[first1:, :]
        else:
            print("In file1, no value equal to ", valueref1, " was found on the specified column: ", vectorref1)
        # Find indices where the value is equal to valueref for file2 in the column vectorref2
        i_2 = np.where(vref2 == valueref2)[0]
        if len(i_2) > 0:
            first2 = i_2[0]
            print("In file2, the first value equal to ", valueref2, " is at i: ", first2)
            new2 = data2[first2:, :]
        else:
            print("In file2, no value equal to ", valueref2, " was found on the specified column: ", vectorref2)


        #Plot
        if (op1 == 0 and op2 == 0):
            t_1 = 10e-6 * np.arange(new1[:, t1].shape[0]) #an array from 0 to the number of rows of new in microseconds
            plt.plot(t_1[0:10000], new1[0:10000, vector1], label='Signal 1')
            t_2 = 10e-6 * np.arange(new2[:, t2].shape[0]) #an array from 0 to the number of rows of new in microseconds
            plt.plot(t_2[0:10000], new2[0:10000, vector2], label='Signal 2')
        elif (op1 == 0 and op2 == 1): #signal1 converted from samples to time
            plt.plot(new2[0:10000, t2], new2[0:10000, vector2], label='Signal 2')
            t_1 = 10e-6 * np.arange(new1[:, 0].shape[0]) + new2[0, t2]
            plt.plot(t_1[0:5000], new1[0:5000, vector1], label='Signal 1')
        elif (op1 == 1 and op2 == 0):
            plt.plot(new1[0:10000, t1], new1[0:10000, vector1], label='Signal 1')
            t_2 = 10e-6 * np.arange(new2[:, 0].shape[0]) + new1[0, t1]
            plt.plot(t_2[0:5000], new2[0:5000, vector2], label='Signal 2')
        else:
            print("At least one op must be 0")
        plt.xlabel('Time (s)')
        plt.ylabel('signal unit')
        plt.title('Comparison of signal1 and signal2')
        plt.legend()
        plt.show()

        # Example usage, we compare when from v_o_ref is 10 (valueref = 10), t1 and t2 are 0 always except the case when is included in a vector which is not the 0 vector
        #d1 = 'sr_v_050424_1.csv'  v_o (vector1 = 2), v_o_ref (vectorref1 = 7), from plecs (op1 = 1), time vector (t1 = 0)        
        #d2 = 'sr_r_050424_1.csv'  v_o (vector2 = 1), v_o_ref (vectorref2 = 6), from trace (op2 = 0), no time vector (t2 = 0)
        #plot_compare( d1, d2, 2, 1, 7, 6, 10, 1, 0, 0, 0):


        
        
    # ========================================================================
