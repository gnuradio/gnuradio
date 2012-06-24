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

function plotfftk_db (all_data, k, sample_rate)

  if (nargin == 2)
    sample_rate = 1.0;
  endif;

  if ((m = nargchk (2,3,nargin)))
    usage (m);
  endif;

  len = 1024;
  data = all_data(k*len + 1 : (k+1)*len);
  ## s = fft (data.*kaiser(len, 5));
  ## s = fft (data.*hamming(len));
  s = fft (data.*hanning(len));

  incr = sample_rate/len;
  min_x = -sample_rate/2;
  max_x = sample_rate/2 - incr;
  plot (min_x:incr:max_x, 20 * log10(abs(fftshift(s))));

endfunction
