"""
Module ``cuk_controller``
==========================


"""
import lrssoc
import struct

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
            0 : {'id':0, 'if':None},
            'startup' : {'id':1, 'if':Startup()},
            'ol' : {'id':2, 'if':OL()},
            'energy' : {'id':3, 'if':Energy()},
            }


class OL:
    def __init__(self):
        pass
    

    def set(self, params):

        u = params['u']
        data = list(struct.pack('<f', u))
        
        return data


    def get(self, data):

        pars = struct.unpack('<f', data)

        params = {
            'u': pars[0]
            }

        return params


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


class Energy:
    def __init__(self):
        pass
    

    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']

        a0 = params['a0']
        a1 = params['a1']
        a2 = params['a2']

        b1 = params['b1']
        b2 = params['b2']

        en = params['en']
        
        data = list(struct.pack('<ffffffff', k1, k2, a0, a1, a2, b1, b2, en))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffffffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'a0': pars[2],
            'a1': pars[3],
            'a2': pars[4],
            'b1': pars[5],
            'b2': pars[6],
            'en': pars[7],
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
