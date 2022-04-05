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

from gnuradio import analog  # just need analog for the enum
from .psk import (
    create_encodings,
    psk_mod,
    psk_demod,
    psk_constellation,
)
from .qam import (
    is_power_of_four,
    get_bit,
    get_bits,
    make_differential_constellation,
    make_non_differential_constellation,
    large_ampls_to_corners_mapping
)
from .qamlike import qam32_holeinside_constellation
from .bpsk import bpsk_constellation, dbpsk_constellation
from .qpsk import qpsk_constellation, dqpsk_constellation
from .gmsk import gmsk_mod, gmsk_demod
from .gfsk import gfsk_mod, gfsk_demod
from .cpm import cpm_mod
from .modulation_utils import *
from .ofdm_txrx import ofdm_tx, ofdm_rx
from .soft_dec_lut_gen import (
    soft_dec_table,
    show_table,
    soft_dec_table_generator,
    calc_soft_dec,
    calc_soft_dec_from_table,
)
from .psk_constellations import *
from .qam_constellations import *
from .constellation_map_generator import constellation_map_generator
from . import packet_utils
from generic_mod_demod import generic_mod, generic_demod


class gmskmod_bc(cpmmod_bc):
    def __init__(self, samples_per_sym=2, L=4, beta=0.3):
        cpmmod_bc.__init__(self, analog.cpm.GAUSSIAN,
                           0.5, samples_per_sym, L, beta)
