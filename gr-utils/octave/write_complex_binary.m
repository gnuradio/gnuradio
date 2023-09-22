%
% Copyright 2001 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
% 
%

function v = write_complex_binary (data, filename)

  %% usage: write_complex_binary (data, filename)
  %%
  %%  open filename and write data to it
  %%  Format is interleaved float IQ e.g. each
  %%  I,Q 32-bit float IQIQIQ....
  %%  This is compatible with read_complex_binary()
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
