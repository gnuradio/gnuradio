%
% Copyright 2001 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% GNU Radio is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 3, or (at your option)
% any later version.
%
% GNU Radio is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with GNU Radio; see the file COPYING.  If not, write to
% the Free Software Foundation, Inc., 51 Franklin Street,
% Boston, MA 02110-1301, USA.
%

function v = write_complex_binary (data, filename)

  %% usage: write_complex_binary (data, filename)
  %%
  %%  open filename and write data to it
  %%  Format is interleaved float IQ e.g. each
  %%  I,Q 32-bit float IQIQIQ....
  %%  This is compatabile with read_complex_binary()
  %%

  m = nargchk (2,2,nargin);
  if (m)
    usage (m);
  end

  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    re = real(data);
    im = imag(data);
    re = re(:)';
    im = im(:)';
    y = [re;im];
    y = y(:);
    v = fwrite (f, y, 'float');
    fclose (f);
  end
end
