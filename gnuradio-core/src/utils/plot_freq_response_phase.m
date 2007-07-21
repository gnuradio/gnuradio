#
# Copyright 2001 Free Software Foundation, Inc.
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

function plot_freq_response_db (b,...)
  if (nargin == 1)
    ## Response of an FIR filter
    a = 1;
  elseif (nargin == 2)
    ## response of an IIR filter
    a = va_arg ();
  endif

  [H,w] = freqz (b,a);

  grid;
  xlabel ('Normalized Frequency (Fs == 1)');
  ylabel ('Phase (radians)');
  abs = abs(H);
  plot (w/(2*pi), atan2(imag(H), real(H)))
endfunction
