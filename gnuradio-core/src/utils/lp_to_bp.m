#
# Copyright 2002 Free Software Foundation, Inc.
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

## transform low pass coefficients into bandpass coefficients

function bp_taps = lp_to_bp (lp_taps, center_freq, sampling_freq)
  arg = 2 * pi * center_freq / sampling_freq;
  bp_taps = lp_taps .* exp (j*arg*(0:length(lp_taps)-1)');
endfunction
