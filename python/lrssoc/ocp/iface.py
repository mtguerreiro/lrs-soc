"""
Module ``iface``
================

"""
import socket
import time
import struct
import lrssoc

class Commands:
    """
    """
    def __init__(self):
        self.trace_read = 0
        self.trace_reset = 1
        self.trace_get_size = 2
        self.trace_set_size = 3
        self.trace_get_number_signals = 4
        self.trace_get_signals_names = 5
        self.trace_get_number_traces = 6
        self.trace_get_traces_names = 7
        self.trace_get_address = 8
        self.cs_status = 9
        self.cs_enable = 10
        self.cs_disable = 11
        self.cs_controller_if = 12
        self.cs_hardware_if = 13
        self.cs_get_number_controllers = 14
        self.cs_get_controllers_names = 15
        self.platform_id = 16
        self.platform_if = 17

        
class Interface:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------

        
    """
    def __init__(self, comm_type='ethernet', settings={}):

        self.hwp = lrssoc.hwp.Protocol(comm_type=comm_type, settings=settings)
        
        self.cmd = Commands()


    def trace_read(self, tr_id):
        """Reads the selected trace.

        All data is interpreted as floating point.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        Raises
        ------
        TypeError
            If `tr_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the data received, or an error code. If the
            command was executed successfully, status is zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')
      
        cmd = self.cmd.trace_read

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )
       
        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_read.__name__
            print('{:}: Error reading trace. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        #n = round( len(data) / 4 )
        #fmt = '<' + 'f' * n
        #data = struct.unpack(fmt, data)

        return (0, data)


    def trace_reset(self, tr_id):
        """Gets the number of bytes saved for the selected trace.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        size : int
            Trace's size. The maximum size is determined by the controller.
            
        Raises
        ------
        TypeError
            If `tr_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element an error code, if any. If the command was executed
            successfully, status is zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')
      
        cmd = self.cmd.trace_reset

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )
       
        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_reset.__name__
            print('{:}: Error resetting trace. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        return (0,)

    
    def trace_get_size(self, tr_id):
        """Gets the number of bytes saved for the selected trace.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        Raises
        ------
        TypeError
            If `id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the size, or an error code. If the command was
            executed successfully, status is zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')
        
        cmd = self.cmd.trace_get_size

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_get_size.__name__
            print('{:}: Error getting trace size. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        size = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, size)


    def trace_set_size(self, tr_id, size):
        """Sets the size, in bytes, of the selected trace.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        size : int
            Trace's size, in bytes. The maximum size is determined by the
            controller.
            
        Raises
        ------
        TypeError
            If `tr_id` or `size` are not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element an error code, if any. If the command was executed
            successfully, status is zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')

        if type(size) is not int:
            raise TypeError('`size` must be of int type.')
        
        cmd = self.cmd.trace_set_size

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
       
        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_set_size.__name__
            print('{:}: Error setting trace size. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        return (0,)


    def trace_get_number_signals(self, tr_id):
        """Gets the number of signals stored in the selected trace.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        Raises
        ------
        TypeError
            If `tr_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the number of signals, or an error code. If the
            command was executed successfully, status is zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')
        
        cmd = self.cmd.trace_get_number_signals

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_get_number_signals.__name__
            print('{:}: Error getting number of signals. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        n = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, n)


    def trace_get_signals_names(self, tr_id):
        """Gets the description of the signals stored in the selected trace.

        Parameters
        ----------
        tr_id : int
            Trace's ID. This ID must exist in the controller, otherwise an
            error will be returned.

        Raises
        ------
        TypeError
            If `tr_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is a list with the name of each signal, or an
            error code. If the command was executed successfully, status is
            zero.
            
        """
        if type(tr_id) is not int:
            raise TypeError('`tr_id` must be of int type.')
        
        cmd = self.cmd.trace_get_signals_names

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(tr_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.trace_get_signals_names.__name__
            print('{:}: Error getting names of signals. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        names = data.split(b'\x00')
        if names[-1] == b'': names = names[:-1]

        return (0, names)
    

    def trace_get_number_traces(self):
        """Gets the number of traces stored in the controller.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the number of traces, or an error code. If the
            command was executed successfully, status is zero.
            
        """       
        cmd = self.cmd.trace_get_number_traces

        status, data = self.hwp.request(cmd)

        if status < 0:
            funcname = Interface.trace_get_number_traces.__name__
            print('{:}: Error getting number of traces. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        n = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, n)


    def trace_get_traces_names(self):
        """Gets the description of the traces stored in the controller.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is a list with the name of each trace, or an error
            code. If the command was executed successfully, status is zero.
            
        """
        cmd = self.cmd.trace_get_traces_names

        status, data = self.hwp.request(cmd)

        if status < 0:
            funcname = Interface.trace_get_traces_names.__name__
            print('{:}: Error getting names of traces. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        names = data.split(b'\x00')
        if names[-1] == b'': names = names[:-1]

        return (0, names)


    def cs_status(self, cs_id):
        """Gets the status of the selected control system.

        Parameters
        ----------
        cs_id : int
            Control system's ID. This ID must exist in the controller,
            otherwise an error will be returned.

        Raises
        ------
        TypeError
            If `cs_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is control systems's status or an error code. If
            the command was executed successfully, status is zero.
            
        """
        if type(cs_id) is not int:
            raise TypeError('`cs_id` must be of int type.')
        
        cmd = self.cmd.cs_status

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cs_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.cs_status.__name__
            print('{:}: Error getting control system status. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        status = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, status)


    def cs_enable(self, cs_id):
        """Enables the selected control system.

        Parameters
        ----------
        cs_id : int
            Control system's ID. This ID must exist in the controller,
            otherwise an error will be returned.

        Raises
        ------
        TypeError
            If `cs_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element an error code, if any. If the command was executed
            successfully, status is zero.
            
        """
        if type(cs_id) is not int:
            raise TypeError('`cs_id` must be of int type.')
        
        cmd = self.cmd.cs_enable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cs_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.cs_enable.__name__
            print('{:}: Error enabling control system. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0,)
    

    def cs_disable(self, cs_id):
        """Disables the selected control system.

        Parameters
        ----------
        cs_id : int
            Control system's ID. This ID must exist in the controller,
            otherwise an error will be returned.

        Raises
        ------
        TypeError
            If `cs_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element an error code, if any. If the command was executed
            successfully, status is zero.
            
        """
        if type(cs_id) is not int:
            raise TypeError('`cs_id` must be of int type.')
        
        cmd = self.cmd.cs_disable

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cs_id, msb=False) )

        status, data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.cs_disable.__name__
            print('{:}: Error disabling control system. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0,)


    def cs_controller_if(self, cs_id, data=None):
        """Sends data to the selected controller's interface.

        Parameters
        ----------
        cs_id : int
            Control system's ID. This ID must exist in the controller,
            otherwise an error will be returned.

        data : list
            Data to be sent. By default, it is `None`.

        Raises
        ------
        TypeError
            If `cs_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the controller's response or an error code, if
            any. If the command was executed successfully, status is zero.
            
        """
        if type(cs_id) is not int:
            raise TypeError('`cs_id` must be of int type.')
        
        cmd = self.cmd.cs_controller_if

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cs_id, msb=False) )
        if data is not None: tx_data.extend( data )

        status, rx_data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.cs_controller_if.__name__
            print('{:}: Error accessing the controller\'s interface. Error {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0, rx_data)


    def cs_hardware_if(self, cs_id, data=None):
        """Sends data to the selected controller's hardware interface.

        Parameters
        ----------
        cs_id : int
            Control system's ID. This ID must exist in the controller,
            otherwise an error will be returned.

        data : list
            Data to be sent. By default, it is `None`.

        Raises
        ------
        TypeError
            If `cs_id` is not of `int` type.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the controller's response or an error code, if
            any. If the command was executed successfully, status is zero.
            
        """
        if type(cs_id) is not int:
            raise TypeError('`cs_id` must be of int type.')
        
        cmd = self.cmd.cs_hardware_if

        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cs_id, msb=False) )
        if data is not None: tx_data.extend( data )

        status, rx_data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.cs_hardware_if.__name__
            print('{:}: Error accessing the controller\'s hardware interface. Error {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0, rx_data)


    def cs_get_number_controllers(self):
        """Gets the number of control systems stored in the controller.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the number of controllers, or an error code. If
            the command was executed successfully, status is zero.
            
        """       
        cmd = self.cmd.cs_get_number_controllers

        status, data = self.hwp.request(cmd)

        if status < 0:
            funcname = Interface.cs_get_number_controllers.__name__
            print('{:}: Error getting number of controllers. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        n = lrssoc.conversions.u8_to_u32(data, msb=False)

        return (0, n)


    def cs_get_controllers_names(self):
        """Gets the description of the controllers stored in the controller.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is a list with the name of each controller, or an
            error code. If the command was executed successfully, status
            is zero.
            
        """
        cmd = self.cmd.cs_get_controllers_names

        status, data = self.hwp.request(cmd)

        if status < 0:
            funcname = Interface.cs_get_controllers_names.__name__
            print('{:}: Error getting names of controllers. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        names = data.split(b'\x00')
        if names[-1] == b'': names = names[:-1]

        return (0, names)


    def platform_id(self):
        """Gets a description of the connected platform.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the platform's ID, or an error code. If the
            command was executed successfully, status is zero.
            
        """
        cmd = self.cmd.platform_id

        status, data = self.hwp.request(cmd)

        if status < 0:
            funcname = Interface.platform_id.__name__
            print('{:}: Error getting the platform\'s ID. Error code {:}\r\n'.format(funcname, status))
            return (-1, status)

        if data[-1] == b'': data = data[:-1]

        return (0, data)
    

    def platform_if(self, data=None):
        """Sends data to the platform's interface.

        Parameters
        ----------
        data : list
            Data to be sent. By default, it is `None`.

        Returns
        -------
        tuple
            A tuple, where the first element is the command's status and the
            second element is the controller's response or an error code, if
            any. If the command was executed successfully, status is zero.
            
        """
        cmd = self.cmd.platform_if

        tx_data = []
        if data is not None: tx_data.extend( data )

        status, rx_data = self.hwp.request(cmd, tx_data)

        if status < 0:
            funcname = Interface.platform_if.__name__
            print('{:}: Error accessing the platforms\'s interface. Error {:}\r\n'.format(funcname, status))
            return (-1, status)
        
        return (0, rx_data)
