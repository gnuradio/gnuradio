t = read_complex_binary('msdd.dat',10000000);

fftsize=256;
w = [0:pi/fftsize:pi];

for i = 0:length(t)/fftsize
	fftdata = fft(i*fftsize:i*fftsize+fftsize);
	clear plot;
	plot(w,fftdata);
endfor

pause;
