# Perform circular correlation via FFT
function corr = fftcorr(v,ref)
    corr = ifft(conj(fft(ref)).*fft(v));
endfunction
