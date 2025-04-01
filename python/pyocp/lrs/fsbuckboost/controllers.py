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
        self.buck_sfb = _BuckSFB(2, ctl_if)
        self.boost_energy = _BoostEnergy(3, ctl_if)


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


class _BuckSFB(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'ki', 'kv', 'k_ev', 'dt',
            'filt_en', 'alpha'
            )
        self._model_params = ModelParams


    def get_model_params(self):

        return self._model_params
    

    def set_model_params(self, params):

        self._model_params = params

        
    def set_gains(self, ts=1e-3, os=5, dt=1/100e3):

        params = self._get_gains(ts=ts, os=os, dt=dt)

        return self.set_params(params)        


    def _get_gains(self, ts=1e-3, os=5, dt=1/100e3):

        # Model
        V_in = self._model_params.v_in
        R = self._model_params.R
        L = self._model_params.L
        Co = self._model_params.Co
        
        A = np.array([
            [0,         -1 / L],
            [1 / Co,    -1 / R / Co]
            ])
        
        B = np.array([
            [V_in / L],
            [0]
            ])

        C = np.array([0, 1])

        # Augmented model (integrator)
        Aa = np.zeros((3,3))
        Aa[:2, :2] = A
        Aa[2, :2] = -C

        Ba = np.zeros((3,1))
        Ba[:2, 0] = B[:, 0]

        # Poles
        zeta = -np.log(os/100) / np.sqrt( np.pi**2 + (np.log(os/100))**2 )
        wn = 4 / ts / zeta

        p1 = -zeta * wn + 1j * wn * np.sqrt(1 - zeta**2)
        p2 = p1.conj()
        p3 = 5 * p1.real
        p = [p1, p2, p3]
        
        # Controller design
        K = scipy.signal.place_poles(Aa, Ba, p).gain_matrix[0]
        
        ki = K[0]
        kv = K[1]
        k_ev = K[2]
        
        return {'ki':ki, 'kv':kv, 'k_ev':k_ev, 'dt':dt}
    

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


class _BoostEnergy(pyocp.controller.ControllerTemplate):
    
    def __init__(self, ctl_id, ctl_if):
        super().__init__(ctl_id, ctl_if)

        self.keys = (
            'k1', 'k2', 'k3', 'dt',
            'C', 'L', 'alpha', 'filt_en',
            'kd',
            'soe_en',
            'sc_soe_k1', 'sc_soe_k2', 'sc_soe_gain'
            )
        self._model_params = ModelParams
        

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


    def set_gains_soe_controller(self, ts=1, os=1, pv_gain=1/50):

        params = self._get_gains_soe(ts, os=os, pv_gain=pv_gain)
        
        return self.set_params(params)


    def _get_gains_soe(self, ts, os=1, pv_gain=1/50):

        zeta = -np.log(os / 100) / np.sqrt( np.pi**2 + (np.log(os / 100))**2 )
        wn = 4 / ts / zeta
        
        k1 = 2 * zeta * wn
        k2 = -wn**2
        gains = {'sc_soe_k1':k1, 'sc_soe_k2':k2, 'sc_soe_gain':pv_gain}

        print('Gains: {:}'.format(gains))

        return gains
    

    def set_gains_voltage_controller(self, ts=2e-3, os=5, dt=1/100e3):

        params = self._get_gains_voltage_controller(ts, os=os, dt=dt)
        
        return self.set_params(params)
    

    def _get_gains_voltage_controller(self, ts, os=5, method='approx', alpha=5.0, dt=1.0):

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
        #print('Pole placement.\nMethod: {:}'.format(method))
        #print('Poles: {:}'.format(poles))
        
        # Augmented model        
        A = np.array([[ 0.0, 1.0, 0.0],
                      [ 0.0, 0.0, 0.0],
                      [-1.0, 0.0, 0.0]])

        B = np.array([[0.0], [1.0], [0.0]])

        # Gains
        K = scipy.signal.place_poles(A, B, poles).gain_matrix.reshape(-1)

        gains = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'dt':dt}

        #print('Gains: {:}'.format(gains))

        return gains
