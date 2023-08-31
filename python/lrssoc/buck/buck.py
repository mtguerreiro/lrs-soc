"""
Module ``buck``
==============


"""
import lrssoc

class Commands:
    """
    """
    def __init__(self):
        self.blink = 0
        self.adc_en = 1
        self.adc_config = 2
        self.pwm_config = 3


class Buck:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, cs_id, comm, comm_settings):

        self._cs_id = 0
        self._ocp_if = lrssoc.hwi.Interface(comm_type=comm, settings=comm_settings)
        self._ctl_if = lrssoc.buck.buck_controller.BuckController(ocp_if=self._ocp_if, cs_id=cs_id)


    def cs_enable(self):
        """
        """
        status, = self._ocp_if.cs_enable( self._cs_id )

        return status
        

    def cs_disable(self):
        """
        """
        status, = self._ocp_if.cs_disable( self._cs_id )

        return status


    def cs_status(self):
        """
        """
        status, = self._ocp_if.cs_status( self._cs_id )

        return status
        

    def controller_disable(self):
        """
        """
        status, = self._ctl_if.set( 0 )

        return status

    
    def controller_enable(self, controller, reset=True):
        """
        """
        status, = self._ctl_if.reset( controller )
        if status < 0:
            return status
        
        status, = self._ctl_if.set( controller )
        
        return status


    def controller_set_params(self, controller, params):
        """
        """
        status, new_params = self._ctl_if.get_params( controller )
        if status < 0:
            return status

        for param, val in params.items():
            if param in new_params:
                new_params[param] = val
                
        status, = self._ctl_if.set_params( controller, new_params )
        
        return status

    def controller_get(self):
        """
        """
        status, = self._ctl_if.get()

        return status


