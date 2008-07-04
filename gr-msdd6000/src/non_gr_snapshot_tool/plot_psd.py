#!/usr/bin/python
import struct;
from pylab import *;
import sys;

#a = fft([1,2,3,4]);
#print a;

if(len(sys.argv)<2):
	print "usage ./plot_psd.py <filename>"
	sys.exit(-1);

filename = sys.argv[1];

#a = open("test.out_002", "r");
a = open(filename, "r");
samples = [];
i_arr = [];
q_arr = [];
while(True):
	d = a.read(1024*1024*4);
	if(len(d) <= 0):
		break;
	
	for i in range(0,(len(d))/8):
		[s_i, s_q] = struct.unpack_from("<ff", d, i*8);
		samples.append(complex(s_i, s_q));
		i_arr.append(s_i);
		q_arr.append(s_q);
	break;

fft_data = fftshift(fft(samples));
for i in range(0,len(fft_data)):
	if(fft_data[i] == 0):
		fft_data[i] = 0.01;

plot(20*log10(abs(fft_data)));
figure();
subplot(2,1,1);
plot(i_arr);
subplot(2,1,2);
plot(q_arr);
show();


a.close();


