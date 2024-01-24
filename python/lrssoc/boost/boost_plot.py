"""
Module ``boost_plot``
=====================


"""
import lrssoc
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

class Plot:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self):

        plt.style.use('seaborn-bright')
        matplotlib.rcParams['mathtext.fontset'] = 'cm'
        matplotlib.rcParams['font.family'] = 'Times New Roman'
        matplotlib.rcParams.update({'font.size': 12})
        plt.rc('axes', unicode_minus=False)

        self._l_fs = 11.5
        self._title_fs = 12
        self._lw = 1.5
        self._figsize = (10,6)
        
        self._figsize_energy = (5,7)
        self._figsize_states = (9,7)
        
        
    def measurements(self, data, t=None, fig=None):
        """
        """
        if t is None:
            t = np.arange(data.shape[0])

        if fig is None:
            fig, axes = plt.subplots(nrows=2, ncols=1)
            axes = axes.reshape(-1)
            fig.set_size_inches(self._figsize[0], self._figsize[1])
        else:
            axes = fig.get_axes()
            
        axes[0].cla()
        axes[0].step(t, data[:, 3], label='$i_l$', where='post', lw=self._lw)
        axes[0].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self._l_fs)
        axes[0].grid()
        axes[0].set_ylabel('Current (A)', fontsize=self._l_fs)
        axes[0].set_title('Currents', fontsize=self._title_fs)

        axes[1].cla()
        axes[1].sharex(axes[0])
        axes[1].step(t, data[:, 0], label='$V_{dc,in}$', where='post', lw=self._lw)
        axes[1].step(t, data[:, 1], label='$V_{dc,out}$', where='post', lw=self._lw)
        axes[1].step(t, data[:, 2], label='$V_{out}$', where='post', lw=self._lw)
        axes[1].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self._l_fs)
        axes[1].grid()
        axes[1].set_ylabel('Voltage (V)', fontsize=self._l_fs)
        axes[1].set_title('Voltages', fontsize=self._title_fs)

        plt.tight_layout()
