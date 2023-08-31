"""
Module ``afe``
==============

Application for the active rectifier platform

"""
import lrssoc
import struct

class Commands:
    """
    """
    def __init__(self):
        self.set = 0
        self.get = 1
        self.set_params = 2
        self.get_params = 3


class Controllers:
    """
    """
    def __init__(self):
        self.ctl = {
            'sfb' : {'id':0, 'if':SFB()}
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
    

class BuckController:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, hwi, cs_id=0):

        self.cmd = Commands()
        self.ctl = Controllers().ctl
        self.cs_id = cs_id
        self.hwi = hwi
        

    def set(self, controller):
        """

        Parameters
        ----------

        Raises
        ------

        """
        if controller not in self.ctl:
            print('Uknown controller')
            return -1
        
        cmd = self.cmd.set
        cs_id = self.cs_id

        ctl_id = self.ctl[controller]['id']

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )

        status, _ = self.hwi.cs_controller_if(cs_id, tx_data)

        if status < 0:
            funcname = BuckController.set.__name__
            print('{:}: Error setting the controller. Error code {:}\r\n'.format(funcname, status))
            
        return status


    def get(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self.cmd.get
        cs_id = self.cs_id

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, rx_data = self.hwi.cs_controller_if(cs_id, tx_data)

        if status < 0:
            funcname = BuckController.get.__name__
            print('{:}: Error getting the controller. Error code {:}\r\n'.format(funcname, status))
            return (status,)
        
        controller = lrssoc.conversions.u8_to_u32(rx_data, msb=False)
        
        return (status, controller)


    def set_params(self, controller, params):
        """

        Parameters
        ----------

        Raises
        ------
        """
        if controller not in self.ctl:
            print('Uknown controller')
            return -1

        cmd = self.cmd.set_params
        cs_id = self.cs_id

        ctl_id = self.ctl[controller]['id']
        ctl_data = self.ctl[controller]['if'].set(params)

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )
        tx_data.extend( ctl_data )

        status, _ = self.hwi.cs_controller_if(cs_id, tx_data)

        if status < 0:
            funcname = BuckController.set_params.__name__
            print('{:}: Error setting controller params. Error code {:}\r\n'.format(funcname, status))
            
        return status


    def get_params(self, controller):
        """
        """
        if controller not in self.ctl:
            print('Uknown controller')
            return -1

        cmd = self.cmd.get_params
        cs_id = self.cs_id

        ctl_id = self.ctl[controller]['id']

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )

        status, rx_data = self.hwi.cs_controller_if(cs_id, tx_data)   

        if status < 0:
            funcname = BuckController.set_params.__name__
            print('{:}: Error setting controller params. Error code {:}\r\n'.format(funcname, status))
            return (status,)

        params = self.ctl[controller]['if'].get(rx_data)
        
        return (status, params)
