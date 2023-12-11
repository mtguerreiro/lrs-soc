"""
Module ``cuk_controller``
==========================


"""
import lrssoc
import struct
import numpy as np
import scipy.signal
import control

class References:
    """
    """
    def __init__(self):
        pass
    

    def encode(self, ref):

        r = float(ref)
        data = list(struct.pack('<f', r))

        return data

    
    def decode(self, data):

        ref = struct.unpack('<f', data)[0]
        
        return ref


class Controllers:
    """
    """
    def __init__(self):
        self.ctl = {
            0            : {'id':0, 'if':None},
            'startup'    : {'id':1, 'if':Startup()},
            'ol'         : {'id':2, 'if':OL()},
            'energy'     : {'id':3, 'if':Energy()},
            'energy_int' : {'id':4, 'if':EnergyInt()},
            'sfb'        : {'id':5, 'if':SFB()},
            'sfb_int'    : {'id':6, 'if':SFBINT()},
            'pch'        : {'id':7, 'if':PCH()},
            }


class OL:
    def __init__(self):
        pass
    

    def set(self, params):

        u = params['u']
        data = list(struct.pack('<f', u))
        
        return data


    def get(self, data):

        pars = struct.unpack('<f', data)

        params = {
            'u': pars[0]
            }

        return params


class Startup:
    def __init__(self):
        pass
    

    def set(self, params):

        uinc = params['uinc']
        ufinal = params['ufinal']
        data = list(struct.pack('<ff', uinc, ufinal))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ff', data)

        params = {
            'uinc': pars[0],
            'ufinal': pars[1]
            }

        return params


class Energy:
    def __init__(self):
        pass
    

    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']

        a0 = params['a0']
        a1 = params['a1']
        a2 = params['a2']

        b1 = params['b1']
        b2 = params['b2']

        en = params['notch_en']
        
        data = list(struct.pack('<ffffffff', k1, k2, a0, a1, a2, b1, b2, en))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffffffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'a0': pars[2],
            'a1': pars[3],
            'a2': pars[4],
            'b1': pars[5],
            'b2': pars[6],
            'notch_en': pars[7],
            }

        return params
    

    def gains(self, ts, os):
        
        k1 = 16 / ts**2 * (np.pi**2 / np.log(os/100)**2 + 1)
        k2 = 8 / ts

        gains = {'k1':k1, 'k2':k2}

        return gains


    def time_resp_from_gains(self, k1, k2):

        ts = 8 / k2
        os = 100 * np.exp( -4 * np.pi / np.sqrt(k1 * ts**2 - 16) )

        return (ts, os)
    

    def discrete_notch(self, fc, Q, dt):
        
        wc = 2 * np.pi * fc
        
        num = [1, 0 , wc**2]
        den = [1, wc/Q, wc**2]
        tf = (num, den)

        num_d, den_d, _ = scipy.signal.cont2discrete(tf, dt)
        num_d = num_d.reshape(-1) / den_d[0]

        filt = {'a0':num_d[0], 'a1':num_d[1], 'a2':num_d[2], 'b1':den_d[1], 'b2':den_d[2]}
        print(num_d, den_d)
        
        return filt        


class EnergyInt:
    def __init__(self):
        pass
    

    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']
        k3 = params['k3']
        dt = params['dt']
        
        a0 = params['a0']
        a1 = params['a1']
        a2 = params['a2']

        b1 = params['b1']
        b2 = params['b2']

        en = params['notch_en']
        
        data = list(struct.pack('<ffffffffff', k1, k2, k3, dt, a0, a1, a2, b1, b2, en))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<ffffffffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'k3': pars[2],
            'dt': pars[3],
            'a0': pars[4],
            'a1': pars[5],
            'a2': pars[6],
            'b1': pars[7],
            'b2': pars[8],
            'notch_en': pars[9],
            }

        return params
    

    def gains(self, ts, os=5, method='approx', alpha=10.0, dt=1.0):

        # Poles
        if method == 'approx':
            zeta = -np.log(os) / np.sqrt(np.pi**2 + (np.log(os))**2)
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
        print('Pole placement.\nMethod: {:}'.format(method))
        print('Poles: {:}'.format(poles))
        
        # Augmented model        
        A = np.array([[ 0.0, 1.0, 0.0],
                      [ 0.0, 0.0, 0.0],
                      [-1.0, 0.0, 0.0]])

        B = np.array([[0.0], [1.0], [0.0]])

        # Gains
        K = scipy.signal.place_poles(A, B, poles).gain_matrix.reshape(-1)

        gains = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'dt':dt}

        return gains
    

    def discrete_notch(self, fc, Q, dt):
        
        wc = 2 * np.pi * fc
        
        num = [1, 0 , wc**2]
        den = [1, wc/Q, wc**2]
        tf = (num, den)

        num_d, den_d, _ = scipy.signal.cont2discrete(tf, dt)
        num_d = num_d.reshape(-1) / den_d[0]

        filt = {'a0':num_d[0], 'a1':num_d[1], 'a2':num_d[2], 'b1':den_d[1], 'b2':den_d[2]}
        print(num_d, den_d)
        
        return filt


class SFB:
    def __init__(self, model_params={}):

        self._params = {}

        self._params['V_in'] = 25
        self._params['Vo'] = 25
        self._params['Po'] = 120

        self._params['L1'] = 100e-6
        self._params['L2'] = 150e-6
        self._params['Cc'] = 9.4e-6
        self._params['Co'] = 330e-6

        self._params['N'] = 5/3

        for p, v in model_params.items():
            if p in self._params:
                self._params[p] = v


    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']
        k3 = params['k3']
        k4 = params['k4']
        
        x1s = params['x1s']
        x2s = params['x2s']
        x3s = params['x3s']
        x4s = params['x4s']

        us = params['us']
        
        data = list(struct.pack('<fffffffff', k1, k2, k3, k4, x1s, x2s, x3s, x4s, us))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<fffffffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'k3': pars[2],
            'k4': pars[3],
            'x1s': pars[4],
            'x2s': pars[5],
            'x3s': pars[6],
            'x4s': pars[7],
            'us': pars[8],
            }

        return params
    

    def params(self, ts, method='approx', model_params={}):

        for p, v in model_params.items():
            if p in self.params:
                self.params[p] = v
                
        V_in = self._params['V_in']
        Vo = self._params['Vo']
        Po = self._params['Po']

        L1 = self._params['L1']
        L2 = self._params['L2']
        Cc = self._params['Cc']
        Co = self._params['Co']

        N = self._params['N']

        # Steady-state
        Io = Po / Vo
        d = Vo / (Vo + N * V_in)
        Vc = V_in + (1 / N) * Vo
        I_1 = Vo * Io / V_in
        I_2 = Io

        # Linearized model
        a11 = -0/L1;                             a12 = 0;                                     a13 = -(1 - d) / L1;     a14 = 0
        a21 = 0;                                 a22 = -0/L2;                                 a23 = N * d / L2;        a24 = -1 / L2
        a31 = (N**2+1)/N**2 * (1 - d) / (Cc);    a32 = -(N**2+1)/N * d / (Cc);                a33 = 0;                 a34 = 0
        a41 = 0;                                 a42 = 1 / Co;                                a43 = 0;                 a44 = Po / (Co * Vo**2)#(-1/11)#

        A = np.array([[a11, a12, a13, a14],
                      [a21, a22, a23, a24],
                      [a31, a32, a33, a34],
                      [a41, a42, a43, a44]])

        b11 = Vc / L1; b21 = N * Vc / L2; b31 = -(N**2 + 1) / N**2 * (I_1 + N * I_2) / Cc; b41 = 0

        B = np.array([ [b11], [b21], [b31], [b41] ])

        C = np.array([0, 0, 0, 1])

        # Poles
        if method == 'approx':
            wn = 2.2 / ts
            
            p1 = -wn
            p2 = 5*p1
            p3 = 8*p1
            p4 = 12*p1

        elif method == 'bessel':
            p1 = (-4.0156 + 5.0723j) / ts
            p2 = np.conj(p1)
            p3 = (-5.5281 + 1.6553j) / ts
            p4 = np.conj(p3)

        elif method == 'itae':
            p1 = (-4.236 + 12.617j) / ts
            p2 = np.conj(p1)
            p3 = (-6.524 + 4.139j) / ts
            p4 = np.conj(p3)

        else:
            print('Unknown method')
            return 0

        poles = [p1, p2, p3, p4]

        
        sys = control.ss(A, B, C, 0)
        print('Pole placement.\nMethod: {:}'.format(method))
        print('Poles: {:}'.format(poles))
        print('Zeros: {:}'.format(sys.zeros()))

        # State feedback
        K = scipy.signal.place_poles(A, B, poles).gain_matrix[0]

        ctl_params = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'k4':K[3],
                      'x1s':I_1, 'x2s':I_2, 'x3s':Vc, 'x4s':Vo,
                      'us':d}

        return ctl_params


class SFBINT:
    def __init__(self, model_params={}):

        self._params = {}

        self._params['V_in'] = 25
        self._params['Vo'] = 25
        self._params['Po'] = 120

        self._params['L1'] = 100e-6
        self._params['L2'] = 150e-6
        self._params['Cc'] = 9.4e-6
        self._params['Co'] = 330e-6

        self._params['N'] = 5/3

        for p, v in model_params.items():
            if p in self._params:
                self._params[p] = v


    def set(self, params):

        k1 = params['k1']
        k2 = params['k2']
        k3 = params['k3']
        k4 = params['k4']
        ke = params['ke']
        dt = params['dt']
        
        x1s = params['x1s']
        x2s = params['x2s']
        x3s = params['x3s']
        x4s = params['x4s']

        us = params['us']
        
        data = list(struct.pack('<fffffffffff', k1, k2, k3, k4, ke, dt, x1s, x2s, x3s, x4s, us))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<fffffffffff', data)

        params = {
            'k1': pars[0],
            'k2': pars[1],
            'k3': pars[2],
            'k4': pars[3],
            'ke': pars[4],
            'dt': pars[5],
            'x1s': pars[6],
            'x2s': pars[7],
            'x3s': pars[8],
            'x4s': pars[9],
            'us': pars[10],
            }

        return params
    

    def params(self, ts, os, method='approx', dt=1/100e3, model_params={}):

        for p, v in model_params.items():
            if p in self.params:
                self.params[p] = v
                
        V_in = self._params['V_in']
        Vo = self._params['Vo']
        Po = self._params['Po']

        L1 = self._params['L1']
        L2 = self._params['L2']
        Cc = self._params['Cc']
        Co = self._params['Co']

        N = self._params['N']

        # Steady-state
        Io = Po / Vo
        d = Vo / (Vo + N * V_in)
        Vc = V_in + (1 / N) * Vo
        I_1 = Vo * Io / V_in
        I_2 = Io

        # Linearized model
        a11 = 0/L1;                              a12 = 0;                                     a13 = -(1 - d) / L1;     a14 = 0
        a21 = 0;                                 a22 = 0/L2;                                  a23 = N * d / L2;        a24 = -1 / L2
        a31 = (N**2+1)/N**2 * (1 - d) / (Cc);    a32 = -(N**2+1)/N * d / (Cc);                a33 = 0;                 a34 = 0
        a41 = 0;                                 a42 = 1 / Co;                                a43 = 0;                 a44 = Po / (Co * Vo**2)#(-1/11)#

        A = np.array([[a11, a12, a13, a14],
                      [a21, a22, a23, a24],
                      [a31, a32, a33, a34],
                      [a41, a42, a43, a44]])

        b11 = Vc / L1; b21 = N * Vc / L2; b31 = -(N**2 + 1) / N**2 * (I_1 + N * I_2) / Cc; b41 = 0

        B = np.array([ [b11], [b21], [b31], [b41] ])

        C = np.array([0, 0, 0, 1])

        # Augmented model
        n_st = A.shape[0]
        Aa = np.zeros((n_st+1, n_st+1))
        Aa[:n_st, :n_st] = A
        Aa[-1, :n_st] = -C

        Ba = np.zeros((n_st + 1, 1))
        Ba[:n_st, 0] = B[:, 0]

        # Poles
        if method == 'approx':
            zeta = -np.log(os/100) / np.sqrt( np.pi**2 + (np.log(os/100))**2 )
            wn = 4 / zeta / ts
            
            p1 = -zeta*wn + wn * np.emath.sqrt(zeta**2 - 1)
            p2 = -zeta*wn - wn * np.emath.sqrt(zeta**2 - 1)

            p3 = 5*p1.real
            p4 = 10*p1.real
            p5 = 15*p1.real

        elif method == 'bessel':
            p1 = (-4.1104 + 6.3142j) / ts
            p2 = np.conj(p1)
            p3 = (-5.9268 + 3.0813j) / ts
            p4 = np.conj(p3)
            p5 = -6.4480 / ts

        elif method == 'itae':
            p1 = (-3.948 + 13.553j) / ts
            p2 = np.conj(p1)
            p3 = (-6.040 + 5.601j) / ts
            p4 = np.conj(p3)
            p5 = -9.394 / ts

        else:
            print('Unknown method')
            return 0

        poles = [p1, p2, p3, p4, p5]

        sys = control.ss(A, B, C, 0)
        print('Pole placement.\nMethod: {:}'.format(method))
        print('Poles: {:}'.format(poles))
        print('Zeros: {:}'.format(sys.zeros()))
        print('Open-loop poles: {:}'.format(sys.poles()))
        
        # State feedback
        K = scipy.signal.place_poles(Aa, Ba, poles).gain_matrix[0]

        ctl_params = {'k1':K[0], 'k2':K[1], 'k3':K[2], 'k4':K[3],
                      'ke':K[4], 'dt':dt,
                      'x1s':I_1, 'x2s':I_2, 'x3s':Vc, 'x4s':Vo,
                      'us':d}

        return ctl_params


class PCH:
    def __init__(self, model_params={}):

        self._params = {}

        self._params['V_in'] = 20
        self._params['Vo'] = 30
        self._params['Po'] = 120

        self._params['L1'] = 100e-6
        self._params['L2'] = 150e-6
        self._params['Cc'] = 9.4e-6
        self._params['Co'] = 330e-6

        self._params['N'] = 5/3

        for p, v in model_params.items():
            if p in self._params:
                self._params[p] = v


    def set(self, params):

        kj2 = params['kj2']
        kj3 = params['kj3']
        kr2 = params['kr2']

        data = list(struct.pack('<fff', kj2, kj3, kr2))
        
        return data
    

    def get(self, data):

        pars = struct.unpack('<fff', data)

        V_in = self._params['V_in']
        Vo = self._params['Vo']
        Po = self._params['Po']

        L1 = self._params['L1']
        L2 = self._params['L2']
        Cc = self._params['Cc']
        Co = self._params['Co']

        N = self._params['N']
        
        params = {
            'kj2': pars[0] / ((N**2+1)/N * L2 * Co),
            'kj3': pars[1] / (N * L2 * Co),
            'kr2': pars[2] / ((N**2+1)/N * L2**2),
            }

        return params
    

    def params(self, kj2=4405286.343612335, kj3=-5988023.952095808, kr2=5286343.612334802, model_params={}):

        for p, v in model_params.items():
            if p in self.params:
                self.params[p] = v
                
        V_in = self._params['V_in']
        Vo = self._params['Vo']
        Po = self._params['Po']

        L1 = self._params['L1']
        L2 = self._params['L2']
        Cc = self._params['Cc']
        Co = self._params['Co']

        N = self._params['N']

        # Steady-state
        kj2p = kj2 * (N**2+1)/N * L2 * Co
        kj3p = kj3 * N * L2 * Co
        kr2p = kr2 * (N**2+1)/N * L2**2

        ctl_params = {'kj2':kj2p, 'kj3':kj3p, 'kr2':kr2p}

        return ctl_params
    

class Controller:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id=0):

        self._ocp_if = ocp_if
        self._ctl_if = lrssoc.controller.controller.Controller(self._ocp_if)

        self._cs_id = cs_id
        self._ctl = Controllers().ctl

        self._ref = References()
        

    def set(self, controller):
        """

        Parameters
        ----------

        Raises
        ------

        """
        if controller not in self._ctl:
            print('Uknown controller')
            return -1

        ctl_id = self._ctl[controller]['id']

        status, = self._ctl_if.set(self._cs_id, ctl_id)

        if status < 0 :
            return (-1, status)
            
        return (0,)


    def get(self):
        """

        Parameters
        ----------

        Raises
        ------

        """

        status, controller = self._ctl_if.get(self._cs_id)
        
        return (status, controller)


    def set_params(self, controller, params):
        """

        Parameters
        ----------

        Raises
        ------
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return (-1,)

        if self._ctl[controller]['if'] is None:
            print('Error setting controller params. Undefined interface\r\n'.format(status))
            return (-1,)

        ctl_id = self._ctl[controller]['id']
        ctl_data = self._ctl[controller]['if'].set(params)

        status = self._ctl_if.set_params(self._cs_id, ctl_id, ctl_data)
          
        return status


    def get_params(self, controller):
        """
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return (-1,)

        if self._ctl[controller]['if'] is None:
            print('Error getting controller params. Undefined interface.\r\n'.format(status))
            return (-1,)

        ctl_id = self._ctl[controller]['id']

        status, data = self._ctl_if.get_params(self._cs_id, ctl_id)

        if status != 0:
            return (-1, status)
        
        params = self._ctl[controller]['if'].get(data)
        
        return (0, params)


    def reset(self, controller):
        """
        """
        if controller not in self._ctl:
            print('Uknown controller')
            return -1

        ctl_id = self._ctl[controller]['id']

        status = self._ctl_if.reset(self._cs_id, ctl_id)
      
        return status


    def set_ref(self, ref):
        """
        """
        ref_data = self._ref.encode( float(ref) )

        status = self._ctl_if.set_ref(self._cs_id, ref_data)

        return status


    def get_ref(self):
        """
        """
        status, data = self._ctl_if.get_ref(self._cs_id)

        if status != 0:
            return (-1, status)
        
        refs = self._ref.decode(data)
        
        return (0, refs)
