%
% Copyright 2001,2008 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
% 
%

function cv = read_cshort_binary (filename, count)

  %% usage: read_cshort_binary (filename, [count])
  %%
  %%  open filename and return the contents, treating them as
  %%  signed short integers
  %%

  m = nargchk (1,2,nargin);
  if (m)
    usage (m);
  end

  if (nargin < 2)
    count = Inf;
  end

  f = fopen (filename, 'rb');
  if (f < 0)
    cv = 0;
  else
    v = fread (f, count, 'short');
    fclose (f);
    v_r = v(1:2:end);
    v_i = v(2:2:end)*j;
    clear v;
    cv = v_r+v_i;
  end
