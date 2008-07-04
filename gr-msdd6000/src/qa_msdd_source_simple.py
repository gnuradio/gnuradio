#!/usr/bin/python

from pylab import *;
#from scipy.fftpack import fftshift;

import math;

from gnuradio import msdd,gr;


tb = gr.top_block();


src = msdd.source_simple("10.45.4.43",0);
convert = gr.interleaved_short_to_complex();
sink = gr.vector_sink_c();

gain = 40;

fc = 2.4e9;

src.set_decim_rate(8);
#src.set_rx_freq(0,3500000000);
src.set_rx_freq(0,fc);
src.set_pga(0,gain);


tb.connect(src, convert, sink);


tb.start();

v = []
for i in range(0,10000):
	b = math.sqrt(i);
	v.append(b);

tb.stop();

#print sink.data();

data = sink.data();

plot(10*log10(fftshift(fft(sink.data()))));
show();
