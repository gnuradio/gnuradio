%
% Copyright 2001,2002 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
% 
%

function v = read_int_binary (filename, count)

  %% usage: read_int_binary (filename, [count])
  %%
  %%  open filename and return the contents, treating them as
  %%  signed integers
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
    v = 0;
  else
    v = fread (f, count, 'int');
    fclose (f);
  end
end
