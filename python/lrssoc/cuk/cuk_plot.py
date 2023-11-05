"""
Module ``cuk_plot``
===================


"""
import lrssoc
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
        self.figsize = (6.3,2.25)
        self.lw = 1.25
        self.figsize = (8,9)

    
    def plot(self, data):
        """
        """
        plt.figure(figsize=self.figsize)

        ax = plt.subplot(4,1,1)
        plt.plot(data[:, 0], label='$i_i$')
        plt.plot(data[:, 1], label='$i_1$')
        plt.legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        plt.grid()
        plt.ylabel('Current (A)', fontsize=self.l_fs)
        plt.title('Primary currents', fontsize=self.title_fs)

        plt.subplot(4,1,2, sharex=ax)
        plt.plot(data[:, 2], label='$V_{in}$')
        plt.plot(data[:, 3], label='$V_{dc}$')
        plt.plot(data[:, 4], label='$V_{1}$')
        plt.legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        plt.grid()
        plt.ylabel('Voltage (V)', fontsize=self.l_fs)
        plt.title('Primary voltages', fontsize=self.title_fs)
        
        plt.subplot(4,1,3, sharex=ax)
        plt.plot(data[:, 5], label='$i_o$')
        plt.plot(data[:, 6], label='$i_2$')
        plt.legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        plt.grid()
        plt.ylabel('Current (A)', fontsize=self.l_fs)
        plt.title('Secondary currents', fontsize=self.title_fs)

        plt.subplot(4,1,4, sharex=ax)
        plt.plot(data[:, 7], label='$V_{o}$')
        plt.plot(data[:, 8], label='$V_{dc,o}$')
        plt.plot(data[:, 9], label='$V_{2}$')
        plt.legend(loc='upper left', bbox_to_anchor=(1.01,1), borderaxespad=0, handlelength=0.75, handleheight=0.5, handletextpad=0.3, fontsize=self.l_fs)
        plt.grid()
        plt.ylabel('Voltage (V)', fontsize=self.l_fs)
        plt.title('Secondary voltages', fontsize=self.title_fs)

        plt.tight_layout()
