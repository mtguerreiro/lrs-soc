"""
Module ``hwp``
==============

Hardware communication protocol for the LRS-SOC platform. 

It follows the small protocol implemented on the SoC user interface firmware.
For more details, refer to the uiface.c file.

"""
import socket
import lrssoc

class Protocol:

    def __init__(self, comm_type='ethernet', settings={}):

        self.comm = lrssoc.hwc.Comm(comm_type=comm_type, settings=settings)

    
    def request(self, cmd, data=None):
        """Sends a request to the hardware.

        Here, we rely on the fact that the host always replies at least four
        bytes, indicating the status of the command. For more information on
        the expected response, refer to the uiface.c file, which contains a
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
            received, `data` is an empty binary string. 
            
        """
        if type(cmd) is not int:
            raise TypeError('`cmd` must be of `int` type.')
        
        if (type(data) is not list) and (type(data) is not type(None)):
            raise TypeError('`data` must be of `list` or `None` type.')

        if type(data) is list:
            data_size = len(data)
        else:
            data_size = 0

        size = data_size + 4
        tx_data = []
        tx_data.extend( lrssoc.conversions.u32_to_u8(size, msb=False) )
        tx_data.extend( lrssoc.conversions.u32_to_u8(cmd, msb=False) )
        
        if data_size != 0:
            tx_data.extend(data)

        self.comm.connect()
        self.comm.send(tx_data)
        
        status = self.comm.recv(4)
        status = int.from_bytes(status, 'little', signed=True)

        rx_data = b''
        if status > 0:
            size = status
            bytes_rcvd = 0
            while bytes_rcvd < size:
                rec = self.comm.recv(size - bytes_rcvd)
                bytes_rcvd += len(rec)
                rx_data += rec

        self.comm.close()
        
        return (status, rx_data)
