"""
Module ``buck_controller``
==========================


"""
import lrssoc
import struct

class Controllers:
    """
    """
    def __init__(self):
        self.ctl = {
            0 : {'id':0, 'if':None},
            'sfb' : {'id':1, 'if':SFB()}
            }

class SFB:
    def __init__(self):
        pass
    

    def set(self, params):
        k1 = float(params['k1'])
        k2 = float(params['k2'])
        kr = float(params['kr'])
        r = float(params['r'])
        
        data = list(struct.pack('<ffff', kr, k1, k2, r))

        return data
    

    def get(self, data):

        pars = struct.unpack('<ffff', data)

        params = {
            'kr': pars[0],
            'k1': pars[1],
            'k2': pars[2],
            'r': pars[3],
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

        status, = self._ctl_if.reset(self._cs_id, ctl_id)
      
        return (status,)
