#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from build_utils import expand_template, standard_dict
from build_utils_codes import *

import re


# regular blocks
other_roots = [
    'trellis_encoder_XX',
    'trellis_metrics_X',
    'trellis_viterbi_X',
    'trellis_viterbi_combined_X',
    ]
other_signatures = (
    ['bb','bs','bi','ss','si','ii'],
    ['f','c'],
    ['b','s','i'],
    ['b','s','i'],
    )



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

    
