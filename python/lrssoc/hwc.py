"""
Module ``hwc``
==============

Hardware communication interface for the LRS-SOC platform. For now, it
only supports TCP/IP over Ethernet.

"""
import socket

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


    def comm(self, data, n_rx=None):
        """Communicates with the hardware. It sends data and waits for a
        response.

        Here, we rely on the fact that the host (controller) will close the
        socket after receiving the data and sending a reply.

        We'll also rely on the fact that the host always replies at least
        four bytes, indicating the status of the command. If the command
        was properly executed, we should receive 0. Otherwise, we should
        receive -1 (or any other value).

        Parameters
        ----------
        data : list of ints
            Data to be sent. It must be a list of integers, and each integer
            must be between 0 and 255.

        n_rx : int, NoneType
            Number of bytes that are expected to be received. If set to
            `None`, it reads up to 1024 bytes or until the socket on the
            host-side is closed. If set to 0, no data is read.

        Returns
        -------
        rx_data : list
            List with the bytes received.
            
        """
        if type(data) is not list:
            raise TypeError('`data` must be of `str` type.')

        if type(n_rx) is not int and n_rx is not None:
            raise TypeError('`n_rx` must be of `int` or `None` type.')            
        
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((self.host, self.port))
            s.sendall(bytes(data))
            
            if n_rx is None:
                n_rx = 1024

            rx_data = b''
            if n_rx != 0:
                print('Attempting to read {:} bytes'.format(n_rx + 4))
                rx_data = s.recv(n_rx + 4)
                print('Got {:} bytes'.format(len(rx_data)))

        if len(rx_data) < 4:
            #print('No response from host.\n\r')
            return (-1, 0)
                
        reply = int.from_bytes(rx_data[:4], 'little', signed=True)
        if reply != 0:
            #print('Error executing command.')
            #print('Host replied error code {:}.\n\r'.format(reply))
            return (-1, reply)

        return (0, rx_data[4:])


##            # First four bytes is the command status. If we get 0, the command
##            # executed properly. Otherwise, there was an error.
##            rx_data = s.recv(4)
##
##            #print(rx_data)
##
##            if len(rx_data) != 4:
##                #print('No response from host.\n\r')
##                return (-1, 0)
##            
##            reply = int.from_bytes(rx_data, 'little', signed=True)
##            if reply != 0:
##                #print('Error executing command.')
##                #print('Host replied error code {:}.\n\r'.format(reply))
##                return (-1, reply)
##            
##            if n_rx is None:
##                n_rx = 1024
##
##            rx_data = b''
##            if n_rx != 0:
##                print('Attempting to read {:} bytes'.format(n_rx))
##                rx_data = s.recv(n_rx)
##                print('Got {:} bytes'.format(len(rx_data)))

        return (0, rx_data)
