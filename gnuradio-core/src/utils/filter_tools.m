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
%% equivalent to the C++ code in VrComplexFIRfilter::buildFilterComplex

1;

%% hamming window, nothing fancy

function taps = build_filter_complex_lp (gain, ntaps)

  taps = gain * (0.54 - 0.46 * cos (2 * pi * (0:ntaps-1)' / (ntaps-1)));

end

%% old_taps describes a low pass filter, convert it to bandpass
%% centered at center_freq.  center_freq is normalized to Fs (sampling freq)

function new_taps = freq_shift_filter (old_taps, center_freq)

  ntaps = length(old_taps);
  
  new_taps = exp (j*2*pi*center_freq*(0:ntaps-1)') .* old_taps;

end
