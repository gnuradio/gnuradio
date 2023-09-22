%

function [theta, g, g1, k, h] = ofdm_sync(signal,SNR,FFTSIZE,CPLEN)

  rho = SNR/(SNR+1);

  d = abs(signal).^2;
  g = [ zeros(1,FFTSIZE) signal(1:max(size(signal))-FFTSIZE) ];
  g1 = conj(g);
  f = abs(g).^2;
  c = d + f;
  moving_sum_taps = rho/2 * ones(1,CPLEN);
  b = conv(c,moving_sum_taps);
  b = b(1:max(size(signal)));
  %b = b(CPLEN:max(size(b)));
  
  k = g1 .* signal;

  moving_sum_taps2 = ones(1, CPLEN);
  h = conv(k,moving_sum_taps2);
  h = h(1:max(size(signal)));
  %h = h(CPLEN:max(size(h)));
  
  a = abs(h);
  
  theta = a-b;

endfunction
