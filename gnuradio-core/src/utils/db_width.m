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

%% find the N-dB width of the given filter
%% E.g., to find the 3-dB width, use width = db_width (taps, -3.0)
%% the result is normalized to nyquist == 1

function width = db_width (taps, db)
  [H,w] = freqz (taps, 1, 4096);
  Habs = abs(H);
  max_H = max(Habs);
  min_H = min(Habs);
  threshold = max_H * 10^(db/20);
  if (min_H > threshold)
    error ("The %g dB point is never reached", db);
  end
  above = Habs >= threshold;
  width = sum(above) / length (above);
