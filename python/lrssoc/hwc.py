"""
Module ``hwc``
==============

Hardware communication interface for the LRS-SOC platform. For now, it
only supports TCP/IP over Ethernet.

It follows the small protocol implemented on the SoC user interface firmware.
For more details, refer to the uiface.c file.

"""
import socket
import lrssoc

class Comm:
    def __init__(self, comm_type='ethernet', settings={}):

        self.comm_type = comm_type
        self.settigs = settings

        if comm_type == 'ethernet':
            self.comm = Ethernet(settings['host'], settings['port'])

        else:
            raise TypeError('Communication type \'{:}\' not supported.'.format(comm_type))
        
    
    def connect(self):

        self.comm.connect()

    
    def send(self, data):

        self.comm.send(data)


    def recv(self, size):

        data = self.comm.recv(size)

        return data
    

    def close(self):
        
        self.comm.close()
            

class Ethernet:
    """A class to provide an interface to the LRS-SOC platform through
    Ethernet over TCP/IP.

    Parameters
    ----------
    host : str
        IP address of the controller.

    port : int
        Port of the controller. By default, it is `8080`.

    Raises
    ------
    TypeError
        If `host` is not of `str` type.

    TypeError
        If `port` is not of `int` type.
    """
    def __init__(self, host, port=8080):

        if type(host) is not str:
            raise TypeError('`host` must be of `str` type.')
        
        if type(port) is not int:
            raise TypeError('`port` must be of `int` type.')

        self.host = host
        self.port = port
        self.socket = None


    def connect(self):

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.host, self.port))
        

    def send(self, data):

        self.socket.sendall(bytes(data))


    def recv(self, size):

        data = self.socket.recv(size)

        return data
    

    def close(self):

        self.socket.close()
