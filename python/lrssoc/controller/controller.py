"""
Module ``controller``
=====================


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
        self.reset = 4


class Controller:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if):

        self._cmd = Commands()
        self._ocp_if = ocp_if
        

    def set(self, cs_id, ctl_id):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )

        status, _ = self._ocp_if.cs_controller_if(cs_id, tx_data)

        if status < 0:
            print('Error setting the controller. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def get(self, cs_id):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )

        status, rx_data = self._ocp_if.cs_controller_if(cs_id, tx_data)

        if status < 0:
            print('Error getting the controller. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        controller = lrssoc.conversions.u8_to_u32(rx_data, msb=False)
        
        return (0, controller)


    def set_params(self, cs_id, ctl_id, params):
        """

        Parameters
        ----------

        Raises
        ------
        """
        cmd = self._cmd.set_params

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )
        tx_data.extend( params )

        status, _ = self._ocp_if.cs_controller_if(cs_id, tx_data)

        if status < 0:
            print('Error setting controller params. Error code {:}\r\n'.format(status))
            return (-1, status)

        return (0,)


    def get_params(self, cs_id, ctl_id):
        """
        """
        cmd = self._cmd.get_params

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )

        status, rx_data = self._ocp_if.cs_controller_if(cs_id, tx_data)   

        if status < 0:
            print('Error getting controller params. Error code {:}\r\n'.format(status))
            return (-1, status)
       
        return (0, rx_data)


    def reset(self, cs_id, ctl_id):
        """
        """
        cmd = self._cmd.reset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(ctl_id, msb=False) )

        status, _ = self._ocp_if.cs_controller_if(cs_id, tx_data)   

        if status < 0:
            print('Error resetting controller. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)
