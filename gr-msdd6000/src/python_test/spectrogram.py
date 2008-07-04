#!/usr/bin/python

fft_bins = 1024;
stride = 256;

#filename = "output.dat";
#decim = 4;
#Fs = (102.4/decim) * 1e6;


from gnuradio import gr;
from Numeric import *;
import FFT;
import numpy.fft;
from numpy import *;
from pylab import *;
import sys;

if len(sys.argv) <2: 
	print "usage:  %s filename <sample_rate_in_MSPS> <stride_samples>"%(sys.argv[0]);
	sys.exit(-1);

filename = sys.argv[1];
fs = 0;
if(len(sys.argv)>2):
	fs = float(sys.argv[2])*1000000;
print "opening %s.\n"%(filename);

if(len(sys.argv)>=4):
	stride = int(sys.argv[3]);
	print "using stride = %d"%(stride);

tb = gr.top_block();
src = gr.file_source(gr.sizeof_gr_complex, filename, False)
sink = gr.vector_sink_c();
tb.connect(src,sink);
tb.run();

data = sink.data();
dataa = array(data);
datalen = len( data );

time_bins = (datalen - fft_bins) / stride;

print "output vector :: fft_bins = %d, time_bins = %d\n"%(fft_bins,time_bins);

start_idx = 0;

b = numpy.zeros((time_bins, fft_bins), complex);
l = [];

window = numpy.blackman(fft_bins);

for i in range(0,time_bins):
	
	time_chunk = take( dataa, range(start_idx,start_idx + fft_bins), 0);
	time_chunk = time_chunk * window;
	fft_chunk = numpy.fft.fftshift(numpy.fft.fft(time_chunk));
	psd = 10*log10(fft_chunk * conj(fft_chunk)+0.001);

	b[i] = psd.real;
	l.append( psd.real.tolist() );

	start_idx = start_idx + stride;

#c = array(b, 10);

print b[0];
c = array(b);
#l = c.tolist();
print size(l);

x = range(0,time_bins);
print size(x);
y = range(0,fft_bins);
print size(y);

print size(l);

contourf(l);
#contourf([x,y], l);
colorbar();
show();

#print c[1,1];


