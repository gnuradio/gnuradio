#!/bin/env python
# -*- python -*-
#
# Copyright 2003 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import re
from generate_utils import *

roots = ['gr_fir_XXX', 'gr_fir_XXX_generic']


# figure out accumulator type.  Use biggest of input, output and tap type

def code3_to_acc_code (code3):
    if i_code (code3) == 'c' or o_code (code3) == 'c' or tap_code (code3) == 'c':
        return 'c'
    if i_code (code3) == 'f' or o_code (code3) == 'f' or tap_code (code3) == 'f':
        return 'f'
    if i_code (code3) == 'i' or o_code (code3) == 'i' or tap_code (code3) == 'i':
        return 'i'
    return 'i'                          # even short short short needs int accumulator


def code3_to_input_cast (code3):
    if i_code (code3) == 's' and o_code (code3) == 'c':
        return '(float)'
    return ''

def expand_h_cc (root, code3):
    d = init_dict (root, code3)
    expand_template (d, root + '.h.t')
    expand_template (d, root + '.cc.t')

def init_dict (root, code3):
    name = re.sub ('X+', code3, root)
    d = standard_dict (name, code3)
    d['FIR_TYPE'] = 'gr_fir_' + code3
    d['INPUT_CAST'] = code3_to_input_cast (code3)
    acc_code = code3_to_acc_code (code3)
    d['ACC_TYPE'] = char_to_type[acc_code]
    if acc_code == 'c':
        d['N_UNROLL'] = '2'
        d['VRCOMPLEX_INCLUDE'] = '#include <gr_types.h>'
    else:
        d['N_UNROLL'] = '4'
        d['VRCOMPLEX_INCLUDE'] = ''
    return d
    

def generate ():
    for r in roots:
        for s in fir_signatures:
            expand_h_cc (r, s)


if __name__ == '__main__':
    generate ()
