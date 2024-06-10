"""
Module ``boost_controller``
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
            'energyc'    : {'id':2, 'if':Energyc()}, #added for new controller
            'energycint' : {'id':3, 'if':Energycint()}, #added for new controller
            'energycintFPGA' : {'id':4, 'if':EnergycintFPGA()} #added for new controller
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

class Energyc:    #added for new controller
    def __init__(self):
        pass
    

    def set(self, params):

        L = params['L']
        C = params['C']
        K1 = params['K1']
        K2 = params['K2']
        #v_o_ref = params['v_o_ref']
      
        data = list(struct.pack('<ffff', L, C, K1, K2))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffff', data)

        params = {
            'L': pars[0],
            'C': pars[1],
            'K1': pars[2],
            'K2': pars[3]
         #   'v_o_ref': pars[4]
            
            }

        return params

class Energycint:    #added for new controller
    def __init__(self):
        pass
    

    def set(self, params):

        L = params['L']
        C = params['C']
        KI = params['KI']
        K1 = params['K1']
        K2 = params['K2']
        alpha = params['alpha']
        data = list(struct.pack('<ffffff', L, C, KI, K1, K2, alpha))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffffff', data)

        params = {
            'L': pars[0],
            'C': pars[1],
            'KI': pars[2],
            'K1': pars[3],
            'K2': pars[4],
            'alpha':pars[5]
            }

        return params

class EnergycintFPGA:    #added for new controller
    def __init__(self):
        pass
    

    def set(self, params):

        Li = params['Li']
        Co = params['Co']
        K1 = params['K1']
        K2 = params['K2']
        K3 = params['K3'] 
        data = list(struct.pack('<ffffff', Li, Co, K1, K2, K3))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffffff', data)

        params = {
            'Li': pars[0],
            'Co': pars[1],
            'K1': pars[2],
            'K2': pars[3],
            'K3': pars[4]
            }

        return params


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
