#!/usr/bin/env python
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

from build_utils import output_glue

import generate_gr_fir_filter_XXX
import generate_gr_interp_fir_filter_XXX
import generate_gr_rational_resampler_base_XXX
import generate_gr_freq_xlating_fir_filter_XXX
import generate_gr_fir_sysconfig_generic
import generate_gr_fir_sysconfig
import generate_gr_fir_util
import generate_gr_fir_XXX
import generate_gri_fir_filter_with_buffer_XXX

def generate_all():
    generate_gr_fir_XXX.generate()
    generate_gr_fir_filter_XXX.generate()
    generate_gr_interp_fir_filter_XXX.generate()
    generate_gr_rational_resampler_base_XXX.generate()
    generate_gr_freq_xlating_fir_filter_XXX.generate()
    generate_gr_fir_sysconfig_generic.generate()
    generate_gr_fir_sysconfig.generate()
    generate_gr_fir_util.generate()
    generate_gri_fir_filter_with_buffer_XXX.generate()
    output_glue('filter')

if __name__ == '__main__':
    generate_all()
