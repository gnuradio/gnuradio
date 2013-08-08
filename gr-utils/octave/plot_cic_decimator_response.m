#
# Copyright 2004 Free Software Foundation, Inc.
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

function plot_cic_decimator_response (R, N, M)
  ## R = decimation rate
  ## N = number of stages (4)
  ## M = 1
  gain = (R*M)^N
  npoints = 1024;
  w = 0:1/npoints:1-1/npoints;
  w = w * 1 * pi;
  ## w = w * R;
  length(w);
  num = sin (w*R*M/2);
  length (num);
  ## H = sin (w*R*M/2) ./ sin (w/2)
  denom = sin(w/2);
  length (denom);
  H = (num ./ denom) .^ N;
  H(1) = gain;
  H = H ./ gain;
  plot (R*w/(2*pi), 10 * log10 (H));
  ## plot (R*w/(2*pi), H);
endfunction


