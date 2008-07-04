#!/usr/bin/python

address = "10.45.4.43";
fc = 3.5;
decim = 8;
num_avg = 10

fs = 102.4;
packet_size=1400;
gain = 0;
port = 10001;

import math;
import time;
import Numeric;
from gnuradio import msdd,gr,window,wimax;
from pylab import *;

src = msdd.source_simple(address, port)  # build source object

fft_size = 2048;

w = window.blackmanharris(fft_size);
s2v = gr.stream_to_vector(2*gr.sizeof_float, fft_size);
fft = gr.fft_vcc(fft_size, True, w, True);
conj = wimax.conj_vcc(fft_size);
mul = gr.multiply_vcc(fft_size);
norm = wimax.norm_cf();
avg = wimax.average_vXX(gr.sizeof_float, 2*fft_size, num_avg);
v2s = gr.vector_to_stream(2*gr.sizeof_float, fft_size);
#sink = gr.vector_sink_f();

src.set_decim_rate(decim);

src.set_pga(0,gain);
src.set_rx_freq(0,fc);

q = gr.msg_queue(fft_size);
sink = gr.message_sink(gr.sizeof_float, q, True);

tb = gr.top_block();
tb.connect(src,s2v,fft,(mul,0),avg,v2s,norm,sink);
tb.connect(fft,conj,(mul,1));

tb.start();

print "running"

v = []
x = [];

bw = fs/(pow(2,decim));
for i in range(0,fft_size):
	norm_freq = (i - fft_size/2.0)/fft_size;
	bin_frequency = fc + norm_freq * bw;
	x.append( norm_freq );



for i in range(0,fft_size):

	d = q.delete_head();
	d =  d.to_string();
	d =  Numeric.fromstring(d, Numeric.Float32);
	d = 10*log10(d);

	print "plotting\n";
	plot(x,d);
	show();
	print "done\n"




