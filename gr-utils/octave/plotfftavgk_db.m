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

function plotfftavgk_db (all_data, k, sample_rate)

  if (nargin == 2)
    sample_rate = 1.0;
  endif;

  if ((m = nargchk (2,3,nargin)))
    usage (m);
  endif;

  len = 1024;
  ## len = 8192;
  ##window = ones (len, 1);
  #window = kaiser (len, 5);
  window = hanning (len);

  s = zeros (len,1);

  count = min (100, floor (length (all_data)/len));

  for i = 0:count-1;
    data = all_data((k+i)*len + 1 : (k+i+1)*len);
    s = s + abs (fft (data.*window));
  endfor;

  s = s ./ count;

  incr = sample_rate/len;

  if is_complex (all_data);
    min_x = -sample_rate/2;
    max_x = sample_rate/2 - incr;

    x = min_x:incr:max_x;
    y = 20 * log10(fftshift(s));
    plot (x, y);
  else
    min_x = 0
    max_x = sample_rate/2 - incr;

    x = min_x:incr:max_x;
    y = 20 * log10(s(1:len/2));
    plot (x, y);

  endif;


  i = find (y == max(y))
  x(i)
  y(i)

endfunction
