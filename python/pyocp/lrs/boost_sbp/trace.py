import struct
import numpy as np

import pyocp

class Trace(pyocp.trace.TraceTemplate):

    def __init__(self, ocp_if, tr_id=0):

        super().__init__(tr_id=tr_id, ocp_if=ocp_if)
