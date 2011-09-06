#!/usr/bin/env python
#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from build_utils import expand_template, copyright, open_and_log_name
from build_utils_codes import *
import re

# regular blocks

other_roots = [
    'trellis_encoder_XX',
    'trellis_sccc_encoder_XX',
    'trellis_pccc_encoder_XX',
    'trellis_metrics_X',
    'trellis_viterbi_X',
    'trellis_viterbi_combined_XX',
    'trellis_sccc_decoder_X',
    'trellis_sccc_decoder_combined_XX',
    'trellis_pccc_decoder_X',
    'trellis_pccc_decoder_combined_XX',
    ]

other_signatures = (
    ['bb','bs','bi','ss','si','ii'],
    ['bb','bs','bi','ss','si','ii'],
    ['bb','bs','bi','ss','si','ii'],
    ['s','i','f','c'],
    ['b','s','i'],
    ['sb','ss','si','ib','is','ii','fb','fs','fi','cb','cs','ci'],
    ['b','s','i'],
    ['fb','fs','fi','cb','cs','ci'],
    ['b','s','i'],
    ['fb','fs','fi','cb','cs','ci'],
    )


def is_byte (code3):
    if i_code (code3) == 'b' or o_code (code3) == 'b':
        return '1'
    else:
        return '0'


def is_short (code3):
    if i_code (code3) == 's' or o_code (code3) == 's':
        return '1'
    else:
        return '0'


def is_int (code3):
    if i_code (code3) == 'i' or o_code (code3) == 'i':
        return '1'
    else:
        return '0'


def is_float (code3):
    if i_code (code3) == 'f' or o_code (code3) == 'f':
        return '1'
    else:
        return '0'


def is_complex (code3):
    if i_code (code3) == 'c' or o_code (code3) == 'c':
        return '1'
    else:
        return '0'


def standard_dict (name, code3):
    d = {}
    d['NAME'] = name
    d['GUARD_NAME'] = 'INCLUDED_%s_H' % name.upper ()
    d['BASE_NAME'] = re.sub ('^trellis_', '', name)
    d['SPTR_NAME'] = '%s_sptr' % name
    d['WARNING'] = 'WARNING: this file is machine generated.  Edits will be over written'
    d['COPYRIGHT'] = copyright
    d['TYPE'] = i_type (code3)
    d['I_TYPE'] = i_type (code3)
    d['O_TYPE'] = o_type (code3)
    d['TAP_TYPE'] = tap_type (code3)
    d['IS_BYTE'] = is_byte (code3)
    d['IS_SHORT'] = is_short (code3)
    d['IS_INT'] = is_int (code3)
    d['IS_FLOAT'] = is_float (code3)
    d['IS_COMPLEX'] = is_complex (code3)
    return d


def expand_h_cc_i (root, sig):
    # root looks like 'gr_vector_sink_X'
    name = re.sub ('X+', sig, root)
    d = standard_dict (name, sig)
    expand_template (d, root + '.h.t')
    expand_template (d, root + '.cc.t')
    expand_template (d, root + '.i.t')


def generate ():
    i=0
    for r in other_roots :
        for s in other_signatures[i]:
            expand_h_cc_i (r, s)
        i=i+1


if __name__ == '__main__':
    generate ()
