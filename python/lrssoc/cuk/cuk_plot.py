"""
Module ``cuk_plot``
===================


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

        self.l_fs = 11.5
        self.title_fs = 12
        self.lw = 1.25
        self.figsize = (10,6)

    
    def measurements(self, data, t=None, fig=None):
        """
        """
        if t is None:
            t = np.arange(data.shape[0])

        if fig is None:
            fig, axes = plt.subplots(nrows=2, ncols=2)
            axes = axes.reshape(-1)
            fig.set_size_inches(self.figsize[0], self.figsize[1])
        else:
            axes = fig.get_axes()
            
        axes[0].cla()
        axes[0].step(t, data[:, 0], label='$i_i$', where='post')
        axes[0].step(t, data[:, 1], label='$i_1$', where='post')
        axes[0].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[0].grid()
        axes[0].set_ylabel('Current (A)', fontsize=self.l_fs)
        axes[0].set_title('Primary currents', fontsize=self.title_fs)

        axes[1].cla()
        axes[1].sharex(axes[0])
        axes[1].step(t, data[:, 2], label='$V_{in}$', where='post')
        axes[1].step(t, data[:, 3], label='$V_{dc}$', where='post')
        axes[1].step(t, data[:, 4], label='$V_{1}$', where='post')
        axes[1].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[1].grid()
        axes[1].set_ylabel('Voltage (V)', fontsize=self.l_fs)
        axes[1].set_title('Primary voltages', fontsize=self.title_fs)
        
        axes[2].cla()
        axes[2].sharex(axes[0])
        axes[2].step(t, data[:, 5], label='$i_o$', where='post')
        axes[2].step(t, data[:, 6], label='$i_2$', where='post')
        axes[2].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[2].grid()
        axes[2].set_ylabel('Current (A)', fontsize=self.l_fs)
        axes[2].set_title('Secondary currents', fontsize=self.title_fs)

        axes[3].cla()
        axes[3].sharex(axes[0])
        axes[3].step(t, data[:, 7], label='$V_{o}$', where='post')
        axes[3].step(t, data[:, 8], label='$V_{dc,o}$', where='post')
        axes[3].step(t, data[:, 9], label='$V_{2}$', where='post')
        axes[3].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[3].grid()
        axes[3].set_ylabel('Voltage (V)', fontsize=self.l_fs)
        axes[3].set_title('Secondary voltages', fontsize=self.title_fs)

        plt.tight_layout()


    def energy(self, data, t=None, fig=None):
        """
        """
        if t is None:
            t = np.arange(data.shape[0])

        if fig is None:
            fig, axes = plt.subplots(nrows=3, ncols=1)
            axes = axes.reshape(-1)
            fig.set_size_inches(self.figsize[0], self.figsize[1])
        else:
            axes = fig.get_axes()
            
        axes[0].cla()
        axes[0].step(t, data[:, 13+4], label='$P_i$', where='post')
        axes[0].step(t, data[:, 14+4], label='$P_o$', where='post')
        axes[0].step(t, data[:, 15+4], label='$P_c$', where='post')
        axes[0].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[0].grid()
        axes[0].set_ylabel('Power (W)', fontsize=self.l_fs)
        axes[0].set_title('Power', fontsize=self.title_fs)

        axes[1].cla()
        axes[1].sharex(axes[0])
        axes[1].step(t, data[:, 16+4], label='$E$', where='post')
        axes[1].step(t, data[:, 17+4], label='$E^*$', where='post')
        axes[1].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[1].grid()
        axes[1].set_ylabel('Energy (J)', fontsize=self.l_fs)
        axes[1].set_title('Energies', fontsize=self.title_fs)
        
        axes[2].cla()
        axes[2].sharex(axes[0])
        axes[2].step(t, data[:, 12+4], label='$V_c$', where='post')
        axes[2].legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        axes[2].grid()
        axes[2].set_ylabel('Voltage (V)', fontsize=self.l_fs)
        axes[2].set_title('Vc', fontsize=self.title_fs)

        plt.tight_layout()
