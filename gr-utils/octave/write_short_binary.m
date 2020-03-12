%
% Copyright 2001 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
%
%

function v = write_short_binary (data, filename)

  %% usage: write_short_binary (data, filename)
  %%
  %%  open filename and write data to it as 16 bit shorts
  %%

  if ((m = nargchk (2,2,nargin)))
    usage (m);
  endif;

  f = fopen (filename, "wb");
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, "short");
    fclose (f);
  endif;
endfunction;
