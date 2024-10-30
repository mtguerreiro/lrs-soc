import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

import time

import pickle

import scipy.signal

from numpy import savetxt
from numpy import loadtxt


# --- Input ---
port = 8080

host = '131.246.75.129'
settings = {'host':host, 'port':port}

buck = lrssoc.buck.buck.Buck(1, 'ethernet', settings, tr_id=1)

boost = lrssoc.boost.boost.Boost(0, 'ethernet', settings, tr_id=0)

#plot = lrssoc.buck.buck_plot.Plot()

#status, (traces, data, t) = buck.read_trace()


def save_data(file, data):

    with open(file + '.pkl', 'wb') as f:
        pickle.dump(data, f)

def load_data(file):

    with open(file + '.pkl', 'rb') as f:
        data = pickle.load(f)

    return data

#---  composed functions -------

def adc_comp_reset():
    boost._hw_if.set_adc_comp_reset(1)
    boost._hw_if.set_adc_comp_reset(0)
    
def clear_faults():
    boost.clear_hw_status()
    buck.clear_hw_status()

def buck_close_relays():
    buck.set_input_relay(1);
    time.sleep(0.5); 
    buck.set_output_relay(1);

def buck_open_relays():
    buck.set_input_relay(0);
    time.sleep(0.5); 
    buck.set_output_relay(0);

def boost_close_relays():
    boost.set_input_relay(1);
    time.sleep(0.5); 
    boost.set_output_relay(1);

def boost_open_relays():
    boost.set_input_relay(0);
    time.sleep(0.5); 
    boost.set_output_relay(0);


def initialize():
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status();
    buck.get_hw_status(); 
    buck.enable(); 
    buck.get_hw_status();
    
def desenergize(v_o_ref_boost, v_o_final_boost, v_o_ref_buck):
    buck.set_ref(v_o_ref_buck*.8); 
    time.sleep(0.05);
    buck.set_ref(v_o_ref_buck*.6);
    time.sleep(0.05);
    buck.set_ref(v_o_ref_buck*.4);
    time.sleep(0.05);
    buck.set_ref(v_o_ref_buck*.3);
    time.sleep(0.05);
    buck.disable_controller()
    time.sleep(0.1);
    buck_open_relays()
    time.sleep(0.1);
   
    v_o_ref_boost_int = int(v_o_ref_boost)
    v_o_final_boost_int = int(v_o_final_boost)
    k = 1
    for i in range(v_o_ref_boost_int, v_o_final_boost_int, -1):
        boost.set_ref(v_o_ref_boost_int - k);
        k=k+1
        time.sleep(0.05);  
    boost.disable_controller()
    time.sleep(0.1); 
    boost_open_relays()
    
def boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, Ki, K1, K2, alpha):
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status(); 
    boost.set_input_relay(1); 
    time.sleep(0.5); 
    boost.set_output_relay(1); 
    buck.get_hw_status(); 
    buck.enable();
    time.sleep(0.5); 
    boost.set_controller_params('energycint', {'L': 47e-6, 'C': 480e-6, 'KI': Ki, 'K1': K1, 'K2': K2, 'alpha': alpha}); 
    boost.set_ref(v_o_ref_boost); 
    boost.enable_controller('energycint'); 
    time.sleep(0.2); 
    buck.set_input_relay(1);
    time.sleep(0.2);    
    buck.set_output_relay(1); 
    time.sleep(0.2);
    buck.sfb_int_ctl_set_time_resp(1e-3, 5); 
    buck.set_ref(v_o_ref_buck*0.25);
    buck.enable_controller('sfb_int');
    time.sleep(0.1);
    buck.set_ref(v_o_ref_buck*0.5);
    time.sleep(0.1);  
    buck.set_ref(v_o_ref_buck*0.75);
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck);
    time.sleep(0.5);     


def mpc_boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, i_l_min, i_l_max, control_f):
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status(); 
    boost.set_input_relay(1); 
    time.sleep(0.5); 
    boost.set_output_relay(1); 
    buck.get_hw_status(); 
    buck.enable();
    time.sleep(0.5);
    boost.set_controller_params('energycint', {'L': 47e-6, 'C': 480e-6, 'KI': 2.8849e10, 'K1': 2.4342e7, 'K2': 1.0395e4, 'alpha':1e-1}); 
    boost.set_ref(v_o_ref_boost);
    boost.enable_controller('energycint'); 
    time.sleep(0.1);
    boost.set_controller_params('BF_MPC', {'L': 47e-6, 'C': 480e-6, 'i_l_min': i_l_min, 'i_l_max': i_l_max, 'control_f': control_f, 'alpha': 1e-2, 'alpha_l': 0.95});  
    boost.enable_controller('BF_MPC');
    time.sleep(0.1);
    buck.set_input_relay(1);
    time.sleep(0.2);    
    buck.set_output_relay(1); 
    time.sleep(0.2);
    dbuck = v_o_ref_buck / v_o_ref_boost
    buck.set_controller_params('startup', {'uinc': 1e-5, 'ufinal': dbuck});
    buck.enable_controller('startup');
    time.sleep(1);
    buck.sfb_int_ctl_set_time_resp(1e-3, 5); 
    buck.set_ref(v_o_ref_buck);
    buck.enable_controller('sfb_int');
    time.sleep(0.1);



def mpc_boost_powers_buck_seq_plot(v_i, v_o_ref_boost, v_o_ref_buck, i_l_min, i_l_max, control_f):
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status(); 
    boost.set_input_relay(1); 
    time.sleep(0.5); 
    boost.set_output_relay(1); 
    buck.get_hw_status(); 
    buck.enable();
    time.sleep(0.5);
    boost.set_trace_size(200000);
    buck.set_trace_size(200000);    
    boost.set_controller_params('energycint', {'L': 47e-6, 'C': 480e-6, 'KI': 2.8849e10, 'K1': 2.4342e7, 'K2': 1.0395e4, 'alpha':1e-1}); 
    boost.set_ref(v_o_ref_boost);
    boost.enable_controller('energycint'); 
    time.sleep(0.1);
    boost.set_controller_params('BF_MPC', {'L': 47e-6, 'C': 480e-6, 'i_l_min': i_l_min, 'i_l_max': i_l_max, 'control_f': control_f, 'alpha': 1e-2, 'alpha_l': 0.95});  
    boost.enable_controller('BF_MPC');
    time.sleep(0.1);
    buck.set_input_relay(1);
    time.sleep(0.2);    
    buck.set_output_relay(1); 
    time.sleep(0.2);
    dbuck = v_o_ref_buck / v_o_ref_boost
    buck.set_controller_params('startup', {'uinc': 1e-5, 'ufinal': dbuck});
    buck.enable_controller('startup');
    time.sleep(1);
    buck.sfb_int_ctl_set_time_resp(1e-3, 5); 
    buck.set_ref(v_o_ref_buck);
    buck.enable_controller('sfb_int');
    time.sleep(0.1);
    status, (traces, data, t) = boost.read_trace(); 
    status_buck, (traces_buck, data_buck, t_buck) = buck.read_trace();
    desenergize(v_o_ref_buck, v_i + 2, v_o_ref_buck);
    boost.plot(data);
    buck.plot(data_buck);     
    
    
def mpc_desenergize(v_o_ref_boost, v_o_final_boost):
    buck.set_controller_params('startup', {'uinc': -1e-5, 'ufinal': 0.1});
    buck.enable_controller('startup');
    time.sleep(1);
    buck.disable_controller();
    buck_open_relays()
    
    v_o_ref_boost_int = int(v_o_ref_boost)
    v_o_final_boost_int = int(v_o_final_boost)
    k = 1
    for i in range(v_o_ref_boost_int, v_o_final_boost_int, -1):
        boost.set_ref(v_o_ref_boost_int - k);
        k=k+1
        time.sleep(0.05);  
    boost.disable_controller()
    time.sleep(0.1); 
    boost_open_relays()    

    
def boost_powers_buck_seq_plot_boost(v_o_ref_boost, v_o_ref_buck):
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status(); 
    boost.set_input_relay(1); 
    time.sleep(0.5); 
    boost.set_output_relay(1); 
    buck.get_hw_status(); 
    buck.enable();
    time.sleep(0.5); 
    boost.set_trace_size(200000); 
    boost.reset_trace();
    boost.set_controller_params('energycint', {'L': 47e-6, 'C': 480e-6, 'KI': 6.2313e9, 'K1': 8.763e6, 'K2': 6.237e3, 'alpha':1e-2}); 
    boost.set_ref(v_o_ref_boost); 
    boost.enable_controller('energycint'); 
    time.sleep(0.2); 
    buck.set_input_relay(1);
    time.sleep(0.2);    
    buck.set_output_relay(1); 
    time.sleep(0.2);
    buck.sfb_int_ctl_set_time_resp(1e-3, 5); 
    buck.set_ref(v_o_ref_buck*0.25);
    buck.enable_controller('sfb_int');
    time.sleep(0.1);
    buck.set_ref(v_o_ref_buck*0.5);
    time.sleep(0.1);  
    buck.set_ref(v_o_ref_buck*0.75);
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck);
    status, (traces, data, t) = boost.read_trace(); 
    time.sleep(0.5);
    desenergize();
    boost.plot(data); 


def boost_powers_buck_seq_plot_buck(v_o_ref_boost, v_o_ref_buck):
    boost.get_hw_status(); 
    boost.enable(); 
    boost.get_hw_status(); 
    boost.set_input_relay(1); 
    time.sleep(0.5); 
    boost.set_output_relay(1); 
    buck.get_hw_status(); 
    buck.enable();
    time.sleep(0.5); 
    boost.set_controller_params('energycint', {'L': 47e-6, 'C': 480e-6, 'KI': 6.2313e9, 'K1': 8.763e6, 'K2': 6.237e3, 'alpha':1e-2}); 
    boost.set_ref(v_o_ref_boost); 
    boost.enable_controller('energycint'); 
    time.sleep(0.2); 
    buck.set_input_relay(1);
    time.sleep(0.2);    
    buck.set_output_relay(1);
    buck.set_trace_size(200000); 
    buck.reset_trace();
    time.sleep(0.2);
    buck.sfb_int_ctl_set_time_resp(1e-3, 5); 
    buck.set_ref(v_o_ref_buck*0.25);
    buck.enable_controller('sfb_int');
    time.sleep(0.1);
    buck.set_ref(v_o_ref_buck*0.5);
    time.sleep(0.1);  
    buck.set_ref(v_o_ref_buck*0.75);
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck);
    status, (traces, data, t) = buck.read_trace(); 
    time.sleep(0.5);
    desenergize() 
    buck.plot(data); 

    
    





# functions for controller1 (SFB+Integrator)

def seq1_2(file1, v_i, v_o_ref_boost, v_o_ref_boost_2, v_o_ref_buck, Ki, K1, K2, alpha):
    #boost powers buck at v_o_ref_boost with introduced parameters, buck powers resistor at v_o_ref_buck and this remains
    #boost reference changes from v_o_ref_boost to v_o_ref_boost_2
    
    boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, Ki, K1, K2, alpha)
    boost.set_trace_size(200000); 
    boost.reset_trace();
    time.sleep(0.1); 
    boost.set_ref(v_o_ref_boost_2); 
    time.sleep(0.1);  
    desenergize(v_o_ref_boost_2, v_i+2, v_o_ref_buck); 
    status, (traces, data, t) = boost.read_trace(); 
    boost.plot(data); 
    save_data(file1, data);
    
    
def seq1_3(file_boost, file_buck, v_i, v_o_ref_boost, v_o_ref_boost_2, v_o_ref_boost_3, v_o_ref_buck, Ki, K1, K2, alpha):
    #boost powers buck at v_o_ref_boost with introduced parameters, buck powers resistor at v_o_ref_buck and this remains
    #boost reference changes from v_o_ref_boost to v_o_ref_boost_2
    #boost reference changes from v_o_ref_boost_2 to v_o_ref_boost_3
    boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, Ki, K1, K2, alpha)
    boost.set_trace_size(200000); 
    buck.set_trace_size(200000);
    boost.reset_trace();
    buck.reset_trace();
    time.sleep(0.1); 
    boost.set_ref(v_o_ref_boost_2); 
    time.sleep(0.1);
    boost.set_ref(v_o_ref_boost_3); 
    time.sleep(0.1);    
    desenergize(v_o_ref_boost_3, v_i+2, v_o_ref_buck); 
    status, (traces, data, t) = boost.read_trace(); 
    status_buck, (traces_buck, data_buck, t_buck) = buck.read_trace(); 
    boost.plot(data);
    buck.plot(data_buck);    
    save_data(file_boost, data);
    save_data(file_buck, data_buck);


def seq1_2_power(file1, v_i, v_o_ref_boost, v_o_ref_buck, v_o_ref_buck_2, Ki, K1, K2, alpha):
    #boost powers buck at v_o_ref_boost with introduced parameters, buck powers resistor at v_o_ref_buck and this remains
    #buck reference changes from v_o_ref_buck to v_o_ref_buck_2
    
    boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, Ki, K1, K2, alpha)
    boost.set_trace_size(200000); 
    boost.reset_trace();
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck_2); 
    time.sleep(0.1);  
    desenergize(v_o_ref_boost, v_i+2, v_o_ref_buck_2); 
    status, (traces, data, t) = boost.read_trace(); 
    boost.plot(data); 
    save_data(file1, data);
    
    
def seq1_3_power(file_boost, file_buck, v_i, v_o_ref_boost, v_o_ref_buck, v_o_ref_buck_2, v_o_ref_buck_3, Ki, K1, K2, alpha):
    #boost powers buck at v_o_ref_boost with introduced parameters, buck powers resistor at v_o_ref_buck and this remains
    #buck reference changes from v_o_ref_buck to v_o_ref_buck_2
    #buck reference changes from v_o_ref_buck_2 to v_o_ref_buck_3
    
    boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, Ki, K1, K2, alpha)
    boost.set_trace_size(200000); 
    buck.set_trace_size(200000);
    boost.reset_trace();
    buck.reset_trace();
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck_2); 
    time.sleep(0.1);
    buck.set_ref(v_o_ref_buck_3); 
    time.sleep(0.1);    
    desenergize(v_o_ref_boost, v_i+2, v_o_ref_buck_3); 
    status, (traces, data, t) = boost.read_trace(); 
    status_buck, (traces_buck, data_buck, t_buck) = buck.read_trace();
    boost.plot(data);
    buck.plot(data_buck);
    save_data(file_boost, data);
    save_data(file_buck, data_buck);     




# functions for controller2 (MPC)

def mpc_seq1_2(file1, v_i, v_o_ref_boost, v_o_ref_boost_2, v_o_ref_buck, i_l_min, i_l_max, control_f):
    #boost powers buck at v_o_ref_boost, buck powers resistor at v_o_ref_buck and this remains
    #boost controller BF_MPC with introduced parameters takes control and keeps at v_o_ref_boost
    #boost reference changes from v_o_ref_boost to v_o_ref_boost_2

    mpc_boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, i_l_min, i_l_max, control_f); 
    boost.set_trace_size(200000); 
    time.sleep(0.1);
    boost.reset_trace();
    time.sleep(0.1); 
    boost.set_ref(v_o_ref_boost_2); 
    time.sleep(0.1);  
    desenergize(v_o_ref_boost_2, v_i+2, v_o_ref_buck); 
    status, (traces, data, t) = boost.read_trace(); 
    boost.plot(data); 
    save_data(file1, data);
        
  
  

def mpc_seq1_3(file_boost, file_buck, v_i, v_o_ref_boost, v_o_ref_boost_2, v_o_ref_boost_3, v_o_ref_buck, i_l_min, i_l_max, control_f):
    #boost powers buck at v_o_ref_boost, buck powers resistor at v_o_ref_buck and this remains
    #boost controller BF_MPC with introduced parameters takes control and keeps at v_o_ref_boost
    #boost reference changes from v_o_ref_boost to v_o_ref_boost_2

    mpc_boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, i_l_min, i_l_max, control_f); 
    boost.set_trace_size(200000); 
    buck.set_trace_size(200000);
    time.sleep(0.1);
    boost.reset_trace();
    buck.reset_trace();
    time.sleep(0.1); 
    boost.set_ref(v_o_ref_boost_2); 
    time.sleep(0.1);
    boost.set_ref(v_o_ref_boost_3); 
    time.sleep(0.1);     
    desenergize(v_o_ref_boost_3, v_i+2, v_o_ref_buck); 
    status, (traces, data, t) = boost.read_trace();
    status_buck, (traces_buck, data_buck, t_buck) = buck.read_trace();    
    boost.plot(data);
    buck.plot(data_buck);
    save_data(file_boost, data);
    save_data(file_buck, data_buck);



def mpc_seq1_3_power(file_boost, file_buck, v_i, v_o_ref_boost, v_o_ref_buck, v_o_ref_buck_2, v_o_ref_buck_3, i_l_min, i_l_max, control_f):
    #boost powers buck at v_o_ref_boost with introduced parameters, buck powers resistor at v_o_ref_buck and this remains
    #buck reference changes from v_o_ref_buck to v_o_ref_buck_2
    #buck reference changes from v_o_ref_buck_2 to v_o_ref_buck_3
    
    mpc_boost_powers_buck_seq(v_o_ref_boost, v_o_ref_buck, i_l_min, i_l_max, control_f)
    boost.set_trace_size(200000); 
    buck.set_trace_size(200000);
    boost.reset_trace();
    buck.reset_trace();
    time.sleep(0.1); 
    buck.set_ref(v_o_ref_buck_2); 
    time.sleep(0.1);
    buck.set_ref(v_o_ref_buck_3); 
    time.sleep(0.1);    
    desenergize(v_o_ref_boost, v_i+2, v_o_ref_buck_3); 
    status, (traces, data, t) = boost.read_trace();
    status_buck, (traces_buck, data_buck, t_buck) = buck.read_trace();
    boost.plot(data);
    buck.plot(data_buck);
    save_data(file_boost, data);
    save_data(file_buck, data_buck); 
      
