#
# Copyright 2001,2005 Free Software Foundation, Inc.
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

function v = read_xambi (filename)

  ## usage: read_xambi (filename)
  ##
  ## read binary cross-ambiguity data from radar tools.
  ## The file has an 8 float header, the first word of which specifies
  ## the number of doppler bins.
  ## returns a matrix

  if ((m = nargchk (1,1,nargin)))
    usage (m);
  endif;

  f = fopen (filename, "rb");
  if (f < 0)
    v = 0;
  else
    header = fread(f, 8, "float");
    ndoppler_bins = header(1)
    min_doppler = header(2)
    max_doppler = header(3)
    v = fread (f, [ndoppler_bins, Inf], "float");
    fclose (f);
  endif;
endfunction;
