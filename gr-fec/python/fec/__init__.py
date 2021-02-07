#
# Copyright 2012,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks for forward error correction.
'''


try:
    from .fec_python import *
except ImportError:
    import os
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .fec_python import *

# Pybind cannot bind constructors to make functions that return a different type
# Remap make functions to __init__ here

cc_encoder = code.cc_encoder
cc_encoder.__init__ = code.cc_encoder.make
cc_encoder_make = code.cc_encoder.make

cc_decoder = code.cc_decoder
cc_decoder.__init__ = code.cc_decoder.make
cc_decoder_make = code.cc_decoder.make

dummy_encoder = code.dummy_encoder
dummy_encoder.__init__ = code.dummy_encoder.make
dummy_encoder_make = code.dummy_encoder.make

dummy_decoder = code.dummy_decoder
dummy_decoder.__init__ = code.dummy_decoder.make
dummy_decoder_make = code.dummy_decoder.make

repetition_encoder = code.repetition_encoder
repetition_encoder.__init__ = code.repetition_encoder.make
repetition_encoder_make = code.repetition_encoder.make

repetition_decoder = code.repetition_decoder
repetition_decoder.__init__ = code.repetition_decoder.make
repetition_decoder_make = code.repetition_decoder.make

polar_encoder = code.polar_encoder
polar_encoder.__init__ = code.polar_encoder.make
polar_encoder_make = code.polar_encoder.make

polar_encoder_systematic = code.polar_encoder_systematic
polar_encoder_systematic.__init__ = code.polar_encoder_systematic.make
polar_encoder_systematic_make = code.polar_encoder_systematic.make

ccsds_encoder = code.ccsds_encoder
ccsds_encoder.__init__ = code.ccsds_encoder.make
ccsds_encoder_make = code.ccsds_encoder.make

ldpc_encoder_make = ldpc_encoder.make
ldpc_decoder_make = ldpc_decoder.make
tpc_encoder_make = tpc_encoder.make
tpc_decoder_make = tpc_decoder.make

try:
    ldpc_H_matrix = code.ldpc_H_matrix
    ldpc_G_matrix = code.ldpc_G_matrix
    ldpc_par_mtrx_encoder = code.ldpc_par_mtrx_encoder
    ldpc_par_mtrx_encoder_make = ldpc_par_mtrx_encoder.make
    ldpc_par_mtrx_encoder_make_H = ldpc_par_mtrx_encoder.make_H
    ldpc_gen_mtrx_encoder = code.ldpc_gen_mtrx_encoder
    ldpc_gen_mtrx_encoder_make = code.ldpc_gen_mtrx_encoder.make
    ldpc_bit_flip_decoder = code.ldpc_bit_flip_decoder
except AttributeError:
    pass    

polar_decoder_sc = code.polar_decoder_sc
polar_decoder_sc_list = code.polar_decoder_sc_list
polar_decoder_sc_systematic = code.polar_decoder_sc_systematic

from .bitflip import *
from .extended_encoder import extended_encoder
from .extended_decoder import extended_decoder
from .threaded_encoder import threaded_encoder
from .threaded_decoder import threaded_decoder
from .capillary_threaded_decoder import capillary_threaded_decoder
from .capillary_threaded_encoder import capillary_threaded_encoder
from .extended_async_encoder import extended_async_encoder
from .extended_tagged_encoder import extended_tagged_encoder
from .extended_tagged_decoder import extended_tagged_decoder


from .fec_test import fec_test
from .bercurve_generator import bercurve_generator
