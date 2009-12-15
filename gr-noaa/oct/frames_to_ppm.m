% -*- octave -*-
%
% Copyright 2009 Free Software Foundation, Inc.
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

% Extract AVHRR images from HRPT frames.dat
clear

fid = fopen('frames.hrpt');
dat = fread(fid, 'uint16');

frame_len = 11090;

len = floor(length(dat) / frame_len);

chan1 = zeros(len, 2048);

start = 751;
stop = 10986;

for line = 1:len
  chan1(line, 1:2048) = dat(start:5:stop)';
  start = start + frame_len;
  stop = stop + frame_len;
end
 
start = 752;
stop = 10987;

for line = 1:len
  chan2(line, 1:2048) = dat(start:5:stop)';
start = start + frame_len;
stop = stop + frame_len;
end
 
start = 753;
stop = 10988;

for line = 1:len
  chan3(line, 1:2048) = dat(start:5:stop)';
  start = start + frame_len;
  stop = stop + frame_len;
end
 
start = 754;
stop = 10989;

for line = 1:len
  chan4(line, 1:2048) = dat(start:5:stop)';
start = start + frame_len;
stop = stop + frame_len;
end
 
start = 755;
stop = 10990;

for line = 1:len
  chan5(line, 1:2048) = dat(start:5:stop)';
  start = start + frame_len;
  stop = stop + frame_len;
end

colormap(gray) 
saveimage("chan1.ppm", chan1/4, 'ppm')
saveimage("chan2.ppm", chan2/4, 'ppm')
saveimage("chan3.ppm", chan3/4, 'ppm')
saveimage("chan4.ppm", chan4/4, 'ppm')
saveimage("chan5.ppm", chan5/4, 'ppm')
