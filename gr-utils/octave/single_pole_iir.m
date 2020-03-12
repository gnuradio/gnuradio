%
% Copyright 2002 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
%
%

function [b, a] = single_pole_iir (alpha)
  b = [alpha];
  a = [1, alpha - 1];
endfunction