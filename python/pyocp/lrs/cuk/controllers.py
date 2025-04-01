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
    Vi : float = 25
    Vo : float = 25
    Po : float = 120
    
    L1 : float = 100e-6
    L2 : float = 150e-6
    Cc : float = 9.4e-6
    Co : float = 330e-6

    N : float = 5 / 3

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
        self.energy = _Energy(3, ctl_if)
        self.casc_fblin = _CascFblin(4, ctl_if)
        #self.sfb = _SFB(2, ctl_if)
        #self.cascaded = _Cascaded(3, ctl_if)


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


class _Energy(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'k1', 'k2', 'k3', 'dt',
            'a0', 'a1', 'a2', 'b1', 'b2', 'notch_en',
            'C_out',
            'kd'
        )
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params
    

    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params
        

    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin


    def set_notch(self, fc, Q=0.5, dt=1/100e3, enable=False):

        params = self._discrete_notch(fc, Q=Q, dt=dt)

        if enable is False:
            params['notch_en'] = 0
        else:
            params['notch_en'] = 1
        
        return self.set_params(params)
    
        
    def set_gains(self, ts=3e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3, method='approx', alpha=5):

        # Poles
        if method == 'approx':
            zeta = -np.log(os/100) / np.sqrt(np.pi**2 + (np.log(os/100))**2)
            wn = 4/ts/zeta

            p1 = -zeta * wn + wn * np.sqrt(zeta**2 - 1, dtype=complex)
            p2 = np.conj(p1)
            p3 = alpha * p1.real

            poles = [p1, p2, p3]

        elif method == 'bessel':
            p1 = (-3.9668 + 3.7845j) / ts
            p2 = np.conj(p1)
            p3 = -5.0093 / ts

        elif method == 'itae':
            p1 = (-4.35 + 8.918j) / ts
            p2 = np.conj(p1)
            p3 = -5.913 / ts

        else:
            print('Unknown method')
            return 0
            
        poles = [p1, p2, p3]
        
        # Augmented model        
        A = np.array([[ 0.0, 1.0, 0.0],
                      [ 0.0, 0.0, 0.0],
                      [-1.0, 0.0, 0.0]])

        B = np.array([[0.0], [1.0], [0.0]])

        # Gains
        K = scipy.signal.place_poles(A, B, poles).gain_matrix.reshape(-1)

        gains = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'dt':dt}

        return gains


    def _discrete_notch(self, fc, Q, dt):
        
        wc = 2 * np.pi * fc
        
        num = [1, 0 , wc**2]
        den = [1, wc/Q, wc**2]
        tf = (num, den)

        num_d, den_d, _ = scipy.signal.cont2discrete(tf, dt)
        num_d = num_d.reshape(-1) / den_d[0]

        filt = {'a0':num_d[0], 'a1':num_d[1], 'a2':num_d[2], 'b1':den_d[1], 'b2':den_d[2]}
        
        return filt


class _CascFblin(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'ki', 'k_ei', 'kv', 'k_ev', 'dt',
            'kd'
        )
        

    def _decode(self, params_bin):
        
        keys = self.keys
        
        _params = struct.unpack(f'<{len(keys)}f', params_bin)
        params = dict(zip(keys, _params))

        return params
    

    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params
        

    def _encode(self, params):

        keys = self.keys
        
        _params = [params[key] for key in keys]
        params_bin = struct.pack(f'<{len(keys)}f', *_params)

        return params_bin

        
    def set_gains(self, ts=3e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3, method='approx', alpha=5):

        zeta = -np.log(os/100) / np.sqrt(np.pi**2 + (np.log(os/100))**2)
        wn_v = 4 / ( ts * zeta )
        wn_i = 4 / ( (ts / 5 ) * zeta )

        k_ev = -wn_v**2 
        kv = 2 * zeta * wn_v
        
        k_ei = -wn_i**2 
        ki = 2 * zeta * wn_i

        gains = {'ki':ki, 'k_ei':k_ei, 'kv':kv, 'k_ev':k_ev}

        return gains

