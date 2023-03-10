import lrssoc
import struct

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

# --- Input ---
host = '131.246.75.228'
port = 8080
settings = {'host':host, 'port':port}

soc = lrssoc.hwi.Interface(comm_type='ethernet', settings=settings)
