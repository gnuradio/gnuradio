# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks and utilities for digital modulation and demodulation.
'''


# The presence of this file turns this directory into a Python package

import os

try:
    from .digital_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .digital_python import *

from gnuradio import analog # just need analog for the enum
class gmskmod_bc(cpmmod_bc):
    def __init__(self, samples_per_sym = 2, L = 4, beta = 0.3):
        cpmmod_bc.__init__(self, analog.cpm.GAUSSIAN, 0.5, samples_per_sym, L, beta)

from .psk import *
from .qam import *
from .qamlike import *
from .bpsk import *
from .qpsk import *
from .gmsk import *
from .gfsk import *
from .cpm import *
from .modulation_utils import *
from .ofdm_txrx import ofdm_tx, ofdm_rx
from .soft_dec_lut_gen import *
from .psk_constellations import *
from .qam_constellations import *
from .constellation_map_generator import *

from . import packet_utils
