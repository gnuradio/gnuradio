## Copyright (C) 1999,2000  Kai Habel
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA

## -*- texinfo -*-
## @deftypefn {Function File} {} rainbow (@var{n})
## Create color colormap. 
## (red through yellow, green, cyan,blue,magenta to red)
## The argument @var{n} should be a scalar.  If it
## is omitted, the length of the current colormap or 64 is assumed.
## @end deftypefn
## @seealso{colormap}

## Author:  Kai Habel <kai.habel@gmx.de>

## 2001-09-13 Paul Kienzle <pkienzle@users.sf.net>
## * renamed to rainbow for use with tk_octave
## * remove reference to __current_color_map__

function map = rainbow (number)

  if (nargin == 0)
    number = length(colormap);
  elseif (nargin == 1)
    if (! is_scalar (number))
      error ("rainbow: argument must be a scalar");
    endif
  else
    usage ("rainbow (number)");
  endif

  if (number == 1)
    map = [1, 0, 0];  
  elseif (number > 1)
    h = linspace (0, 1, number)';
    map = hsv2rgb ([h, ones(number, 1), ones(number, 1)]);
  else
    map = [];
  endif

endfunction
