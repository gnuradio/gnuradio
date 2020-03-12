%
% Copyright 2001 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
%
%

function plot_freq_response (b,...)
  if (nargin == 1)
    %% Response of an FIR filter
    a = 1;
  elseif (nargin == 2)
    %% response of an IIR filter
    a = va_arg ();
  endif

  [H,w] = freqz (b,a);
  plot (w/(2*pi), abs(H));
  grid;
  xlabel ('Normalized Frequency (Fs == 1)');
  ylabel ('Magnitude (linear)');
endfunction
