"""
Module ``hwc``
==============

Hardware communication interface for the LRS-SOC platform. For now, it
only supports TCP/IP over Ethernet.

"""
import socket
import lrssoc

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


    def comm(self, cmd, data=None):
        """Communicates with the hardware. It sends data and waits for a
        response.

        Here, we rely on the fact that the host (controller) will close the
        socket after receiving the data and sending a reply.

        We'll also rely on the fact that the host always replies at least
        eight bytes, indicating the status of the command and the amount of
        data that it will send. If the command was properly executed, we
        should receive 0. Otherwise, we should receive -1 (or any other value).

        Parameters
        ----------
        cmd : int
            Command to be sent. It must an integers.
        
        data : list of ints, None
            Data to be sent. It must be a list of integers, and each integer
            must be between 0 and 255. Can be set to `None` if there is no
            data to be sent.

        Returns
        -------
        rx_data : list
            List with the bytes received.
            
        """
        if type(cmd) is not int:
            raise TypeError('`cmd` must be of `int` type.')
        
        if (type(data) is not list) and (type(data) is not type(None)):
            raise TypeError('`data` must be of `list` or `None` type.')

        if type(data) is list:
            size = len(data)
        else:
            size = 0
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
        if size != 0:
            tx_data.extend(data)

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:    
            s.connect((self.host, self.port))
            s.sendall(bytes(tx_data))

            status = s.recv(4)
            status = int.from_bytes(status, 'little', signed=True)
            
            size = s.recv(4)
            size = int.from_bytes(size, 'little', signed=True)

            rx_data = b''
            if size != 0:
                bytes_rcvd = 0
                while bytes_rcvd < size:
                    rec = s.recv(size-bytes_rcvd)
                    bytes_rcvd += len(rec)
                    rx_data += rec

        return (status, rx_data)
