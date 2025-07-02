"""
Module ``controllers``
======================


"""
import pyocp
import struct
import numpy as np
import scipy.signal
import control

from dataclasses import dataclass

@dataclass
class ModelParams:
    v_in : float = 20
    R : float = 5
    L : float = 15e-6
    Co : float = 100e-6


class Reference(pyocp.controller.ReferenceTemplate):

    def __init__(self, ctl_if):
        super().__init__(ctl_if)


    def _decode(self, ref_bin):
        
        fmt = '<f'
        ref = struct.unpack(fmt, ref_bin)

        return ref


    def _encode(self, ref):

        fmt = '<f'
        
        ref_bin = struct.pack(fmt, ref)

        return ref_bin


class Controllers:

    def __init__(self, ctl_if):

        self.idle = _Idle(0, ctl_if)
        self.ramp = _Ramp(1, ctl_if)
        

class _Idle(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)


class _Ramp(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = ('u_step', 'u_ref')
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params


    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin
