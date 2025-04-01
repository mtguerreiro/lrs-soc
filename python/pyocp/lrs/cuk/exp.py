import time
import datetime
import numpy as np

import pyocp
import pyocp.data_mng_util as dmu


def config_fsbb(fsbb, run_params):
    
    model_params = run_params['model_params']
    ctl_params = run_params['ctl_params']
    meas_gains = run_params['meas_gains']

    f_pwm = model_params['f_pwm']
    fsbb.hw.set_pwm_frequency(f_pwm)
    
    fsbb.hw.set_meas_gains(meas_gains)

    mp = pyocp.lrs.fsbuckboost.controllers.ModelParams()
    mp.v_in = model_params['V_in']
    mp.R = model_params['R_load']
    mp.L = model_params['L']
    mp.Co = model_params['C_out']
    fsbb.sfb.set_model_params(mp)

    ts = ctl_params['ts']
    os = ctl_params['os']
    dt = 1 / f_pwm
    fsbb.sfb.set_gains(ts=ts, os=os, dt=dt)

    fsbb.set_converter_mode('buck')

    fsbb.trace.set_size(200000)

    # Procedure to enable the controller. If the hardware is run for the first
    # time, the adc will give an invalid measurement that will trigger an error.
    # This procedure enables/disables/enables the controller as a work-around.
    fsbb.idle.enable()

    fsbb.enable()
    fsbb.disable()
    fsbb.hw.clear_status()

    fsbb.idle.enable()
    fsbb.enable()
    time.sleep(0.1)
    status, hw_status = fsbb.hw.get_status()
    if hw_status != 0:
        print('FSBB hw status is set after enabling...')
        return -1

    return 0


def init_fsbb_relays(fsbb):

    fsbb.hw.set_input_relay(1)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(1)


def de_init_bb_relays(fsbb):

    fsbb.hw.set_input_relay(0)
    time.sleep(0.25)
    fsbb.hw.set_output_relay(0)


def config_energy_controller(cuk, ctl_params, model_params):

    f_pwm = model_params['f_pwm']
    cuk.hw.set_pwm_frequency(f_pwm)

    ts = ctl_params['ts']
    os = ctl_params['os']
    cuk.energy.set_gains(ts=ts, os=os, dt=1/f_pwm)

    notch_en = False if ctl_params['notch_en'] == 0 else True
    fc = ctl_params['fc']
    Q = ctl_params['Q']
    cuk.energy.set_notch(fc, Q=Q, dt=1/f_pwm, enable=notch_en)

    kd = ctl_params['kd']
    cuk.energy.set_params({'kd':kd})


def config_casc_fblin_controller(cuk, ctl_params, model_params):

    f_pwm = model_params['f_pwm']
    cuk.hw.set_pwm_frequency(f_pwm)

    ts = ctl_params['ts']
    os = ctl_params['os']
    cuk.casc_fblin.set_gains(ts=ts, os=os, dt=1/f_pwm)

    kd = ctl_params['kd']
    cuk.casc_fblin.set_params({'kd':kd})
    

def set_trace(cuk, trace_mode, trace_params):

    if trace_mode == 0:
        size = trace_params['size']
        cuk.trace.set_mode(0)
        cuk.trace.set_size( size )
    else:
        size = trace_params['size']
        level = trace_params['level']
        signal = trace_params['signal']
        pre_trig_samples = trace_params['pre_trig']
        
        cuk.trace.set_size( size )
        cuk.trace.set_n_pre_trig_samples(pre_trig_samples)
        cuk.trace.set_trig_level(level)
        cuk.trace.set_trig_signal(signal)
    
        cuk.trace.set_mode(1)

    cuk.trace.reset()


def enable_cs(cuk):

    # Procedure to enable the controller. If the hardware is run for the first
    # time, the adc will give an invalid measurement that will trigger an error.
    # This procedure enables/disables/enables the controller as a work-around.
    cuk.idle.enable()

    cuk.enable()
    cuk.disable()
    cuk.hw.clear_status()

    cuk.idle.enable()
    cuk.enable()
    time.sleep(0.1)
    status, hw_status = cuk.hw.get_status()
    if hw_status != 0:
        print('Hw status is set after enabling...')
        return -1

    return 0


def disable_cs(cuk):

    cuk.idle.enable()
    cuk.disable()


def ramp_duty_up(cuk, ramp_params):

    cuk.ramp.set_params({
        'u_step':ramp_params['u_step'],
        'u_ref':ramp_params['u_ref']
        })
    cuk.ramp.reset()
    cuk.ramp.enable()
    time.sleep(0.1)


def ramp_duty_down(cuk):

    cuk.ramp.set_params({'u_ref':0})
    cuk.ramp.enable()
    time.sleep(0.1)


def wait_for_trigger(cuk):

    status, mode = cuk.trace.get_mode()
    if status != 0: return
    if mode == 0: return
    
    while True:
        time.sleep(1)
        status, trig_state = cuk.trace.get_trig_state()
        if trig_state == 4: break


def save_data(cuk, plat, data, meta):

    status, traces = cuk.trace.get_signals()

    meta['traces'] = traces
    
    fname = f'{plat}_{datetime.datetime.now().strftime("%Y%m%d_%H%M%S")}'
    ds = dmu.DataSet()
    ds.data = data
    ds.meta = meta
    ds.source = plat

    dmu.save_data(fname, ds)


def run_cpl_step(settings_cuk, settings_fsbb, run_params, save=False):

    if settings_cuk['host'] == 'localhost':
        plat = 'sim'
        k = 10
    else:
        plat = 'hw'
        k = 1
    
    fsbb = pyocp.lrs.fsbuckboost.iface.Interface('ethernet', settings_fsbb, cs_id=0, tr_id=0)
    config_fsbb(fsbb, run_params['fsbb'])
    init_fsbb_relays(fsbb)

    cuk = pyocp.lrs.cuk.iface.Interface('ethernet', settings_cuk, cs_id=0, tr_id=0)    
    model_params = run_params['cuk']['model_params']
    ctl_params = run_params['cuk']['ctl_params']
    ramp_params = run_params['cuk']['ramp_params']
    exp_params = run_params['cuk']['exp_params']
    exp_params_fsbb = run_params['fsbb']['exp_params']
    
    config_energy_controller(cuk, ctl_params, model_params)
    cuk.trace.set_mode(0)
    cuk.trace.set_size(400000)

    status = enable_cs(cuk)
    if status != 0:
        return (-1, 0)

    ramp_duty_up(cuk, ramp_params)

    time.sleep(1)
    cuk.trace.reset()

    # Prepares load
    fsbb.trace.reset()
    time.sleep(0.2)
    fsbb.set_ref(3)
    fsbb.sfb.reset()
    fsbb.sfb.enable()
    time.sleep(0.3)

    fsbb.set_ref(exp_params_fsbb['v_ref'])
    time.sleep(0.3)

    # Now, load is active, enable energy controller
    cuk.set_ref(exp_params['v_ref'])
    cuk.energy.reset()
    cuk.energy.enable()
    time.sleep(0.2 * k)

    # Load step up
    fsbb.set_ref(exp_params_fsbb['v_ref_step_up'])
    time.sleep(0.3)

    # Voltage step up
    cuk.set_ref(exp_params['v_ref_step_up'])
    time.sleep(0.2 * k)

    cuk.set_ref(exp_params['v_ref'])
    time.sleep(0.2 * k)
    
    fsbb.set_ref(exp_params_fsbb['v_ref'])
    time.sleep(0.3)
    
    # Starts shutting cuk down
    cuk.ramp.enable()
    time.sleep(0.1 * k)    

    # Shuts load down
    fsbb.set_ref(3)
    time.sleep(0.3)
    
    fsbb.idle.enable()
    time.sleep(1)
    fsbb.disable()
        
    ramp_duty_down(cuk)
    time.sleep(0.1 * k)

    time.sleep(4)
    
    disable_cs(cuk)
    time.sleep(0.1 * k)

    status, data_cuk = cuk.trace.read()
    status, data_fsbb = fsbb.trace.read()

    return data_cuk, data_fsbb


def run_ramp_cpl(settings_cuk, settings_fsbb, run_params, save=False):

    if settings_cuk['host'] == 'localhost':
        plat = 'sim'
        k = 10
    else:
        plat = 'hw'
        k = 1
    
    fsbb = pyocp.lrs.fsbuckboost.iface.Interface('ethernet', settings_fsbb, cs_id=0, tr_id=0)
    config_fsbb(fsbb, run_params['fsbb'])
    init_fsbb_relays(fsbb)

    cuk = pyocp.lrs.cuk.iface.Interface('ethernet', settings_cuk, cs_id=0, tr_id=0)    
    model_params = run_params['cuk']['model_params']
    ctl_params = run_params['cuk']['ctl_params']
    ramp_params = run_params['cuk']['ramp_params']
    exp_params = run_params['cuk']['exp_params']
    exp_params_fsbb = run_params['fsbb']['exp_params']
    
    config_energy_controller(cuk, ctl_params, model_params)
    cuk.trace.set_mode(0)
    cuk.trace.set_size(200000)

    status = enable_cs(cuk)
    if status != 0:
        return (-1, 0)

    ramp_duty_up(cuk, ramp_params)

    time.sleep(1)
    cuk.trace.reset()

    # Prepares load
    fsbb.trace.reset()
    time.sleep(0.2)
    fsbb.set_ref(3)
    fsbb.sfb.reset()
    fsbb.sfb.enable()
    time.sleep(0.3)

    fsbb.set_ref(exp_params_fsbb['v_ref'])
    time.sleep(0.3)

    # Shuts load down
    fsbb.set_ref(3)
    time.sleep(0.3)
    
    fsbb.idle.enable()
    time.sleep(1)
    fsbb.disable()
        
    ramp_duty_down(cuk)
    time.sleep(0.1 * k)

    time.sleep(2)
    
    disable_cs(cuk)
    time.sleep(0.1 * k)

    status, data_cuk = cuk.trace.read()
    status, data_fsbb = fsbb.trace.read()

    return data_cuk, data_fsbb


def run_load_step(settings, run_params, save=False, ctlr='energy'):

    if settings['host'] == 'localhost':
        plat = 'sim'
        k = 10
    else:
        plat = 'hw'
        k = 1
    
    cuk = pyocp.lrs.cuk.iface.Interface('ethernet', settings, cs_id=0, tr_id=0)

    model_params = run_params['cuk']['model_params']
    ctl_params = run_params['cuk']['ctl_params']
    ramp_params = run_params['cuk']['ramp_params']
    exp_params = run_params['cuk']['exp_params']

    config_energy_controller(cuk, ctl_params, model_params)
    config_casc_fblin_controller(cuk, ctl_params, model_params)

    trace_mode = 1
    trace_size = 80000
    trace_pre_trig_samples = 100
    trace_trig_level = 0.3
    trace_trig_signal = 10
    trace_params = {
        'size': trace_size,
        'level': trace_trig_level,
        'signal': trace_trig_signal,
        'pre_trig': trace_pre_trig_samples,
    }
    set_trace(cuk, trace_mode=trace_mode, trace_params=trace_params)  

    status = enable_cs(cuk)
    if status != 0:
        return (-1, 0)
    time.sleep(0.1 * k)
    
    ramp_duty_up(cuk, ramp_params)
    time.sleep(0.2 * k)
    
    # Runs the experiment
    print('Running the experiment...')
    cuk.set_ref(exp_params['v_ref'])
    if ctlr == 'energy':
        cuk.energy.reset()
        cuk.energy.enable()
    else:
        cuk.casc_fblin.reset()
        cuk.casc_fblin.enable()
        
    time.sleep(0.1 * k)

    cuk.set_ref(exp_params['v_ref_step_up'])
    time.sleep(0.1 * k)

    cuk.set_ref(exp_params['v_ref'])
    time.sleep(0.1 * k)

    cuk.set_ref(exp_params['v_ref_step_down'])
    time.sleep(0.1 * k)

    cuk.set_ref(exp_params['v_ref'])
    time.sleep(0.1 * k)
    
    # Load switch
    cuk.hw.set_load_switch(1)
    time.sleep(0.1 * k)

    cuk.hw.set_load_switch(0)
    time.sleep(0.1 * k)
    
    ramp_duty_down(cuk)
    time.sleep(0.1 * k)
    
    disable_cs(cuk)
    time.sleep(0.1 * k)
    
    wait_for_trigger(cuk)

    time.sleep(1)    
    status, data = cuk.trace.read()

    if save is True:
        save_data(cuk, plat, data, run_params['cuk'])

    return data
