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

function v = write_float_binary (data, filename)

  ## usage: write_float_binary (data, filename)
  ##
  ##  open filename and write data to it as 32 bit floats
  ##

  if ((m = nargchk (2,2,nargin)))
    usage (m);
  endif;

  f = fopen (filename, "wb");
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, "float");
    fclose (f);
  endif;
endfunction;
