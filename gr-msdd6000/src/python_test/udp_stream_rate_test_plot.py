#!/usr/bin/python

from socket import *
import string
import time
import struct;
from random import *;
import array;
import cmath;
from numpy import *;
from numpy.fft import *;
from pylab import *;

myport = randint(1025,65535);
filename = "output.dat";

msdd_port = 10001
msdd_host = "10.45.4.46"

buf = 100000;

my_udp_addr = ('',randint(1025,65535));

UDPSock = socket(AF_INET,SOCK_DGRAM);
UDPSock.bind(my_udp_addr);

f_mhz = 2500;	

print "fc = %d"%(f_mhz);

f_hz = 0;	
gain = 20; 	# attenuation
window = 3; 	#0=rect, 1=hanning, 2=hamming, 3=blackman

samples = 65535*4*2;
#samples = 16384;
#samples = 16*1024*1024;
#samples = samples*4; 	#bytes of data we are requesting

# decim 0-8 ( 3 - 8 )
#decim = 5;	# rate ok
decim = 8;
decim = decim+16;	# +16 to use 16bit floats instead of 32 bit floats
mode = 0; 	#0=IQ, 1=MAG, 2=MAGDB
#sets = 0;
sets = 0xffffffff;

size_int = 4;
request_len = 6*size_int;	# 6 int items not including the 8 bytes for opcode and length fields
print "request len = %d"%(request_len);

raw_data = struct.pack("<IIIIIIII", 0x01, request_len, f_mhz, f_hz, gain, samples, decim, sets);

data = raw_data;

UDPSock.sendto(data, (msdd_host, msdd_port));

print "sent"



count = 0;

total_data = [];

state = 0;

vals = [];
mags = [];
re = [];

sample_count = 0;
IQ_bytes=0;


numtocap = 1000;
IQ_bytes = 4 * numtocap;

numbytes = 65536*100;
#numbytes = 65536*2;
#numbytes = 1024;

num_rx = 0;
start =  time.time();
l = [];
arr = [];

while(num_rx < numbytes):
	data = UDPSock.recv(1024);
	l.append(data);
	num_rx = num_rx + len(data);


end = time.time();

# send stop command
halt_data = struct.pack(">II", 0x04, 0x00);
UDPSock.sendto(halt_data, (msdd_host, msdd_port));

# perform timing analysis
print "recieved %d bytes in %f sec"%(numbytes, end-start);
bytes_per_sec = numbytes / (end-start);
samples_per_sec = bytes_per_sec / 4;
MSPS = samples_per_sec / 1000000.0;

print "Got %f MSPS"%(MSPS);
print "Expected %f MSPS"%(102.4/math.pow(2,(decim-16)));


# plot data
val_arr = [];
mag_arr = [];
mag_arr2 = [];

print "Repacking data..."
f = open("out.dat","w");
for li in l:
	for p in range(0, len(li)/4):
		[i,q] = struct.unpack_from("<hh", li, p*4);
		val = complex(i,q);
		mag_arr.append((val*conj(val)).real);
		val_arr.append(val);
	        binchunk = struct.pack("<ff",float(val.real), float(val.imag) );
		f.write(binchunk);
f.close();
		

dlen = len(val_arr)-1;
fft_data = [];
for i in range(1, dlen-1024, 1024*1024):

        t_in = [];
        for ind in range(i, i+1024):
                t_in.append(val_arr[ind]);

        tmp = 20*log10(fftshift(fft(t_in)));
        #tmp = (fftshift(fft(t_in)));

        if(len(fft_data) == 0):
                for ind in range(0,1024):
                        fft_data.append( tmp[ind] );
        else:
                for ind in range(0,1024):
                        fft_data[ind] = fft_data[ind] + tmp[ind];




print "Plotting..."
subplot(2,1,1);
plot(mag_arr);
title("T power");
subplot(2,1,2);
plot(10*log10(fft_data));
title("PSD");
show();



UDPSock.close();

