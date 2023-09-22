%
% Copyright 2008 Free Software Foundation, Inc.
%
% This file is part of GNU Radio
%
% SPDX-License-Identifier: GPL-3.0-or-later
%
%

% See Altera Application Note AN455
%
% R  = decimation factor, 8-256
% Fc = passband (one-sided) cutoff frequency
% L  = number of taps

function taps = cic_comp_taps(R, Fc, L)
    M = 1;                                   %% Differential delay
    N = 4;                                   %% Number of stages
    B = 18;                                  %% Coeffi. Bit-width
    Fs = 64e6;                               %% (High) Sampling freq in Hz before decimation
    Fo = R*Fc/Fs;                            %% Normalized Cutoff freq; 0<Fo<=0.5/M;
    p = 2e3;                                 %% Granularity
    s = 0.25/p;                              %% Step size
    fp = [0:s:Fo];                           %% Pass band frequency samples
    fs = (Fo+s):s:0.5;                       %% Stop band frequency samples
    f = [fp fs]*2;                           %% Normalized frequency samples; 0<=f<=1
    Mp = ones(1,length(fp));                 %% Pass band response; Mp(1)=1
    Mp(2:end) = abs( M*R*sin(pi*fp(2:end)/R)./sin(pi*M*fp(2:end))).^N;
    Mf = [Mp zeros(1,length(fs))];
    f(end) = 1;
    h = fir2(L,f,Mf);                        %% Filter length L+1
    taps = h/sum(h);                         %% Floating point coefficients
endfunction
