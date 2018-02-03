// designs a complex tap fir filter akin to the hilbert transformer.
//
// The hilbert transformer is classified as a linear phase fir
// with allpass magnitude response and 90 degree phase response for
// positive frequencies and -90 degrees phase for negative frequencies.
// Or, if you prefer, normalized frequencies between .5 and 1 since
// negative frequencies don't really have much meaning outside the complex
// domain.
//
// Normally one would use the hilbert transformer in one leg of a complex
// processing block and a compensating delay in the other.
//
// This one differs in the following respects:
//  It is low pass with a cutoff of .078125
//  The filter is a lowpass kaiser windowed filter with parameter 3
//  The phase response is 45 degrees for positive frequencies and -45
//   for negative frequencies.
//  The coefficient set is used in one path and the same coefficients
//   are used time reversed in the other. This results in the net effect
//   of +/- 90 degrees as in the usual hilbert application.
//
// The coefficient set can be used in the gnuradio frequency translating
// fir filter for ssb demodulation.
//
// This isn't as computationally efficient as using the hilbert transformer
// and compensating delay but fascinating none the less.
//
// This program is for the scilab language a very powerful free math
// package similar to Matlab with infinitely better price/performance.
//
// compute the prototype lowpass fir
// length is 255 (odd) for the same symmetry reasons as the hilbert transformer

len = 1023;
l2 = floor(len/2);
md = l2 + 1;
l3 = md + 1;

h = wfir( 'lp', len, [10.0/256 0], 'kr', [3 0] );

H = fft(h);

H(1:l2)=H(1:l2)*exp(%i*%pi/4);
H(md)=0+%i*0;
H(l3:len)=H(l3:len)*exp(-%i*%pi/4);

j=real(ifft(H));
k(1:len)=j(len:-1:1);
x=j+%i.*k;
X=fft(x);
plot(abs(X))

f = file('open','taps')
for i=(1:len)
  fprintf( f, '%f%+fj', j(i), k(i) )
end

file('close',f)

