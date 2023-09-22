%
% Copyright 2004 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
%
%

function plot_cic_decimator_response (R, N, M)
  %% R = decimation rate
  %% N = number of stages (4)
  %% M = 1
  gain = (R*M)^N
  npoints = 1024;
  w = 0:1/npoints:1-1/npoints;
  w = w * 1 * pi;
  %% w = w * R;
  length(w);
  num = sin (w*R*M/2);
  length (num);
  %% H = sin (w*R*M/2) ./ sin (w/2)
  denom = sin(w/2);
  length (denom);
  H = (num ./ denom) .^ N;
  H(1) = gain;
  H = H ./ gain;
  plot (R*w/(2*pi), 10 * log10 (H));
  %% plot (R*w/(2*pi), H);
endfunction


