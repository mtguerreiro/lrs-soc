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
        four bytes, indicating the status of the command. For more information
        on the expected response, refer to the uiface.c file, which contains a
        description of the protocol.

        Parameters
        ----------
        cmd : int
            Command to be sent. It must an integer.
        
        data : list of ints, None
            Data to be sent. It must be a list of integers, and each integer
            must be between 0 and 255. Can be set to `None` if there is no
            data to be sent.

        Returns
        -------
        tuple : (status, data)
            Returns a tuple, where the first element is the command status,
            and the second element is the data received. If no data was
            `data` is an empty binary string. 
            
        """
        if type(cmd) is not int:
            raise TypeError('`cmd` must be of `int` type.')
        
        if (type(data) is not list) and (type(data) is not type(None)):
            raise TypeError('`data` must be of `list` or `None` type.')

        if type(data) is list:
            size = len(data)
        else:
            size = 0

        size = size + 4
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        if (size - 4)!= 0:
            tx_data.extend(data)

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            print(tx_data)
            s.connect((self.host, self.port))
            s.sendall(bytes(tx_data))

            status = s.recv(4)
            status = int.from_bytes(status, 'little', signed=True)
            
            rx_data = b''
            if status > 0:
                size = status
                bytes_rcvd = 0
                while bytes_rcvd < size:
                    rec = s.recv(size-bytes_rcvd)
                    bytes_rcvd += len(rec)
                    rx_data += rec

            print(status)
        
        return (status, rx_data)
