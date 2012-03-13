#!/usr/bin/env python
#
# Copyright 2004,2006,2007,2008,2009 Free Software Foundation, Inc.
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

from build_utils import expand_template, standard_dict
from build_utils_codes import *

import re


# sources and sinks
ss_signatures = ['s', 'i', 'f', 'c']

ss_roots = [
    'gr_vector_source_X',
    'gr_vector_sink_X',
    'gr_noise_source_X',
    'gr_sig_source_X',
    'gr_probe_signal_X',
    'gr_probe_signal_vX'
    ]

# regular blocks
reg_signatures = ['ss', 'ii', 'ff', 'cc']

reg_roots = [
    'gr_add_const_XX',
    'gr_sub_XX',
    'gr_divide_XX',
    'gr_mute_XX',
    'gr_add_const_vXX',
    'gr_multiply_const_vXX',
    'gr_integrate_XX',
    'gr_moving_average_XX',
    ]

# other blocks
others = (
    ('gr_chunks_to_symbols_XX',     ('bf', 'bc', 'sf', 'sc', 'if', 'ic')),
    ('gr_unpacked_to_packed_XX',    ('bb','ss','ii')),
    ('gr_packed_to_unpacked_XX',    ('bb','ss','ii')),
    ('gr_xor_XX',                   ('bb','ss','ii')),
    ('gr_and_XX',                   ('bb','ss','ii')),
    ('gr_and_const_XX',             ('bb','ss','ii')),
    ('gr_or_XX',                    ('bb','ss','ii')),
    ('gr_not_XX',                   ('bb','ss','ii')),
    ('gr_sample_and_hold_XX',       ('bb','ss','ii','ff')),
    ('gr_argmax_XX',                ('fs','is','ss')),
    ('gr_max_XX',                   ('ff','ii','ss')),
    ('gr_peak_detector_XX',         ('fb','ib','sb')),
    ('gr_multiply_XX',              ('ss','ii')),
    ('gr_multiply_const_XX',        ('ss','ii')),
    ('gr_add_XX',                   ('ss','cc','ii'))
    )


def expand_h_cc_i (root, sig):
    # root looks like 'gr_vector_sink_X'
    name = re.sub ('X+', sig, root)
    d = standard_dict (name, sig)
    expand_template (d, root + '.h.t')
    expand_template (d, root + '.cc.t')
    expand_template (d, root + '.i.t')


def generate ():
    expand_h_cc_i ('gr_add_const_XX', 'sf')     # for MC4020
    expand_h_cc_i ('gr_vector_sink_X', 'b')
    expand_h_cc_i ('gr_vector_source_X', 'b')
    expand_h_cc_i ('gr_probe_signal_X', 'b')
    expand_h_cc_i ('gr_probe_signal_vX', 'b')
    for r in ss_roots:
        for s in ss_signatures:
            expand_h_cc_i (r, s)
    for r in reg_roots :
        for s in reg_signatures:
            expand_h_cc_i (r, s)
            
    for root, sigs in others:
        for s in sigs:
            expand_h_cc_i (root, s)



if __name__ == '__main__':
    generate ()

    
