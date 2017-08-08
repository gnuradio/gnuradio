% Copyright (C) 2017 Free Software Foundation
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

% GNU Octave script to accompany the GNURadio flowgraph
%
%     gnuradio/gr-digital/examples/demod/symbol_sync_test_float.grc
%
% to simulate the Mueller and Muller and Gardner timing error detectors
% and compute Expected TED gains applicable to the situation in that
% flowgraph.

% Define some constants
N_bits = 30000;               % Number of bits used for timing error analysis
Rb = 1200.0;
bits_per_sym = 1;
baud_rate = Rb/bits_per_sym;
N_syms = N_bits/bits_per_sym; % Number of symbols used for timing error analysis
tea_sps = 100;                % samples/symbol used for timing error analysis
tea_Fs = tea_sps * baud_rate; % sample rate used for timing error analysis
a_sps = 6.66667;              % actual samples per symbol used in application
a_Fs = a_sps * baud_rate;     % actual sample rate used in application

% Create rectangular pulse filter
pf_taps = 1/tea_sps * ones(tea_sps, 1);
pf_len = length(pf_taps);
pf_delay = pf_len/2;

% Generate some bits
bits = rand(N_bits,1) > 0.5;
%bits = repmat([1;0], [N_bits/2, 1]);

% Convert the bits to NRZ symbols in [-1, 1]
symbols = 2*bits-1;

% Convert symbols to baseband pulses
pkg load signal;
x = tea_sps * upsample(symbols, tea_sps);
baseband = filter(pf_taps, [1], x);

% Create rectangular matched filter,
% that's a little too long (7 sps vs 6.667 sps), to introduce more ISI
isi_sps = round((tea_sps/a_sps * 7) - tea_sps); % Play with isi samples to see S-curve effects
if (mod(isi_sps, 2) != 0)
    isi_sps = isi_sps + 1;
end
mf_taps = 1/(tea_sps+isi_sps) * ones((tea_sps+isi_sps), 1);
mf_len = length(mf_taps);
mf_delay = mf_len/2;

% Matched filter the received baseband pulses
mf_baseband = filter(mf_taps, [1], baseband);
mf_baseband = mf_baseband * 1.0; % Play with amplitude to see S-curve effects

% Symbol centers are now at indices n*tea_sps + isi_sps/2 (I think!)
% Symbol clock period is tea_sps samples
% Symbol peaks are perfectly at +/-1.0

% Timing offset granularity is in 1/tea_sps-th of a symbol
tau = [(-tea_sps/2):1:(tea_sps/2-1)];
tau_norm = tau/tea_sps*a_sps;

% M&M TED. Contellation points at -1.0 and +1.0.
% Gardener TED.
% Perfect estimate of symbol clock period.
% No external noise.
mm_ted_output = zeros(N_syms-2, length(tau));
ga_ted_output = zeros(N_syms-2, length(tau));
% For each known symbol peak set (M&M needs prev and current symbol)
for i = [2:(N_syms-1)]
    % Cycle through all the timing offsets around this symbol
    % using a perfect symbol clock period estimate
    opt_prev_idx = (i-1)*tea_sps + isi_sps/2;
    opt_curr_idx = i*tea_sps + isi_sps/2;
    for j = [1:length(tau)]
        prev_soft_sym = mf_baseband(opt_prev_idx - tau(j));
        mid_soft_samp = mf_baseband((opt_curr_idx+opt_prev_idx)/2 - tau(j));
        curr_soft_sym = mf_baseband(opt_curr_idx - tau(j));
        if (prev_soft_sym >= 0)
            prev_decision = 1;
        else
            prev_decision = -1;
        end
        if (curr_soft_sym >= 0)
            curr_decision = 1;
        else
            curr_decision = -1;
        end
        mm_ted_output(i,j) = prev_decision * curr_soft_sym - curr_decision * prev_soft_sym;
        ga_ted_output(i,j) = (prev_soft_sym - curr_soft_sym) * mid_soft_samp;
    end
end

mean_mm_ted_output = mean(mm_ted_output);
mean_ga_ted_output = mean(ga_ted_output);

% Plot the S-Curves
figure(1);
plot(tau_norm, mean_mm_ted_output, '- .b;M and M TED;', ...
     tau_norm, mean_ga_ted_output, '- +r;Gardner TED;');
title('S-Curve, 6.667 Samples/Symbol, Rectangular Pulses, Imperfect MF, E_s/N_0 = \infty');
xlabel('Timing Error from Symbol Center, \tau_\epsilon (samples)');
ylabel('Expected Value of TED Output, E(e[n] | \tau_\epsilon)');
grid on;

% Plot the TED gains
figure(2);
tau_diff = tau(1:(end-1))+0.5;
tau_diff_norm = tau_diff/tea_sps*a_sps;
diff_mm_ted_output = diff(mean_mm_ted_output)/(a_sps/tea_sps);
diff_ga_ted_output = diff(mean_ga_ted_output)/(a_sps/tea_sps);
plot(tau_diff_norm, diff_mm_ted_output, '- .b;M and M TED;', ...
     tau_diff_norm, diff_ga_ted_output, '- +r;Gardner TED;');
title('TED Gain, 6.667 Samples/Symbol, Rectangular Pulses, Imperfect MF, E_s/N_0 = \infty');
xlabel('Timing Error from Symbol Center, \tau_\epsilon (samples)');
ylabel('Timing Error Detector Gain, Slope of E(e[n] | \tau_\epsilon), (sample^{-1})');
grid on;

% Print out the central TED gains
k = length(diff_mm_ted_output);
mm_ted_gain = mean(diff_mm_ted_output(((k-1)/2):((k-1)/2+1)))
k = length(diff_ga_ted_output);
ga_ted_gain = mean(diff_ga_ted_output(((k-1)/2):((k-1)/2+1)))

