#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Processing blocks common to many flowgraphs.
'''


import os

try:
    from .blocks_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .blocks_python import *

from .stream_to_vector_decimator import *
from .msg_meta_to_pair import meta_to_pair
from .msg_pair_to_var import msg_pair_to_var
from .var_to_msg import var_to_msg_pair

#alias old add_vXX and multiply_vXX
add_vcc = add_cc
add_vff = add_ff
add_vii = add_ii
add_vss = add_ss
multiply_vcc = multiply_cc
multiply_vff = multiply_ff
multiply_vii = multiply_ii
multiply_vss = multiply_ss

# Compatibility layer for transition to gr-pdu. Scheduled for removal in 3.11.
from .pdu_compatibility import *
