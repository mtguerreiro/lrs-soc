"""
Module ``buck_controller``
===========================


"""
import lrssoc
import struct
import numpy as np
import scipy.signal
import control

class References:
    """
    """
    def __init__(self):
        pass
    

    def encode(self, ref):

        r = float(ref)
        data = list(struct.pack('<f', r))

        return data

    
    def decode(self, data):

        ref = struct.unpack('<f', data)[0]
        
        return ref


class Controllers:
    """
    """
    def __init__(self):
        self.ctl = {
            0            : {'id':0, 'if':None},
            'startup'    : {'id':1, 'if':Startup()},
            'sfb_int'    : {'id':2, 'if':SFBINT()},
            }


class Startup:
    def __init__(self):
        pass
    

    def set(self, params):

        uinc = params['uinc']
        ufinal = params['ufinal']
        data = list(struct.pack('<ff', uinc, ufinal))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ff', data)

        params = {
            'uinc': pars[0],
            'ufinal': pars[1]
            }

        return params


class SFBINT:
    def __init__(self, model_params={}):

        self._params = {}

        self._params['V_in'] = 12

        self._params['L'] = 47e-6
        self._params['Co'] = 150e-6

        self._params['R_load'] = 3.34

        for p, v in model_params.items():
            if p in self._params:
                self._params[p] = v


    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']
        ke = params['ke']
        dt = params['dt']
                
        data = list(struct.pack('<ffff', k1, k2, ke, dt))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'ke': pars[2],
            'dt': pars[3],
            }

        return params
    

    def params(self, ts, os, method='approx', dt=1/100e3, model_params={}):

        for p, v in model_params.items():
            if p in self.params:
                self.params[p] = v
                
        V_in = self._params['V_in']

        L = self._params['L']
        Co = self._params['Co']
        R_load = self._params['R_load']

        a11 = 0; a12 = -1 / L;
        a21 = 1 / Co; a22 = - 1 / R_load / Co
        A = np.array([[a11, a12],
                      [a21, a22]])

        b11 = V_in / L; b21 = 0

        B = np.array([ [b11], [b21] ])

        C = np.array([0, 1])

        # Augmented model
        n_st = A.shape[0]
        Aa = np.zeros((n_st+1, n_st+1))
        Aa[:n_st, :n_st] = A
        Aa[-1, :n_st] = -C

        Ba = np.zeros((n_st + 1, 1))
        Ba[:n_st, 0] = B[:, 0]

        # Poles
        if method == 'approx':
            zeta = -np.log(os/100) / np.sqrt( np.pi**2 + (np.log(os/100))**2 )
            wn = 4 / zeta / ts
            
            p1 = -zeta*wn + wn * np.emath.sqrt(zeta**2 - 1)
            p2 = -zeta*wn - wn * np.emath.sqrt(zeta**2 - 1)

            p3 = 5*p1.real

        #elif method == 'bessel':
        #    p1 = (-4.1104 + 6.3142j) / ts
        #    p2 = np.conj(p1)
        #    p3 = (-5.9268 + 3.0813j) / ts

        #elif method == 'itae':
        #    p1 = (-3.948 + 13.553j) / ts
        #    p2 = np.conj(p1)
        #    p3 = (-6.040 + 5.601j) / ts

        else:
            print('Unknown method')
            return 0

        poles = [p1, p2, p3]

        sys = control.ss(A, B, C, 0)
        print('Pole placement.\nMethod: {:}'.format(method))
        print('Poles: {:}'.format(poles))
        print('Zeros: {:}'.format(sys.zeros()))
        print('Open-loop poles: {:}'.format(sys.poles()))
        
        # State feedback
        K = scipy.signal.place_poles(Aa, Ba, poles).gain_matrix[0]

        ctl_params = {'k1':K[0], 'k2':K[1], 'ke':K[2], 'dt':dt}

        return ctl_params

    
class Controller:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id=0):

        self._ocp_if = ocp_if
        self._ctl_if = lrssoc.controller.controller.Controller(self._ocp_if)

        self._cs_id = cs_id
        self._ctl = Controllers().ctl

        self._ref = References()
        

    def set(self, controller):
        """

        Parameters
        ----------

        Raises
        ------

        """
        if controller not in self._ctl:
            print('Uknown controller')
            return -1

        ctl_id = self._ctl[controller]['id']

        status, = self._ctl_if.set(self._cs_id, ctl_id)

        if status < 0 :
            return (-1, status)
            
        return (0,)


    def get(self):
        """

        Parameters
        ----------

        Raises
        ------

        """

        status, controller = self._ctl_if.get(self._cs_id)
        
        return (status, controller)


    def set_params(self, controller, params):
        """

        Parameters
        ----------

        Raises
        ------
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return (-1,)

        if self._ctl[controller]['if'] is None:
            print('Error setting controller params. Undefined interface\r\n'.format(status))
            return (-1,)

        ctl_id = self._ctl[controller]['id']
        ctl_data = self._ctl[controller]['if'].set(params)

        status = self._ctl_if.set_params(self._cs_id, ctl_id, ctl_data)
          
        return status


    def get_params(self, controller):
        """
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return (-1,)

        if self._ctl[controller]['if'] is None:
            print('Error getting controller params. Undefined interface.\r\n'.format(status))
            return (-1,)

        ctl_id = self._ctl[controller]['id']

        status, data = self._ctl_if.get_params(self._cs_id, ctl_id)

        if status != 0:
            return (-1, status)
        
        params = self._ctl[controller]['if'].get(data)
        
        return (0, params)


    def reset(self, controller):
        """
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return -1

        ctl_id = self._ctl[controller]['id']

        status = self._ctl_if.reset(self._cs_id, ctl_id)
      
        return status


    def set_ref(self, ref):
        """
        """
        ref_data = self._ref.encode( float(ref) )

        status = self._ctl_if.set_ref(self._cs_id, ref_data)

        return status


    def get_ref(self):
        """
        """
        status, data = self._ctl_if.get_ref(self._cs_id)

        if status != 0:
            return (-1, status)
        
        refs = self._ref.decode(data)
        
        return (0, refs)
