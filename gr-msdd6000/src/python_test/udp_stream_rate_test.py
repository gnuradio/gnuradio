#!/usr/bin/python

from socket import *
import string
import time
import struct;
import random;
import array;
import cmath;
from numpy import *;
from numpy.fft import *;
from pylab import *;

myport = random.randint(1025,65535);
filename = "output.dat";

msdd_port = 10001
msdd_host = "10.45.4.46"

buf = 100000;

my_udp_addr = ('',10001);
my_udp_addr = ('10.45.1.229 ',10001);

UDPSock = socket(AF_INET,SOCK_DGRAM);
UDPSock.bind(my_udp_addr);

#f_mhz = 3500;	
#f_mhz = 3500;	
f_mhz = 1000;	
f_hz = 0;	
gain = 0; 	
window = 3; 	#0=rect, 1=hanning, 2=hamming, 3=blackman

#samples = 65535;
samples = 16384;
#samples = samples*4; 	#bytes of data we are requesting

decim = 2;	#0-8   (3 => 2^3 = 8)   # ok
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
IQ_bytes = numtocap * numtocap;

numbytes = 1000 * 65536;

num_rx = 0;
start = -1;
while(num_rx < numbytes):
	data = UDPSock.recv(65536);

	if(start==-1):
		start =  time.time();

	num_rx = num_rx + len(data);
#	print num_rx;


end = time.time();
print "recieved %d bytes in %f sec"%(numbytes, end-start);

bytes_per_sec = numbytes / (end-start);
samples_per_sec = bytes_per_sec / 4;
MSPS = samples_per_sec / 1000000.0;

print "Got %f MSPS"%(MSPS);
print "Expected %f MSPS"%(102.4/math.pow(2,(decim-16)));
	

halt_data = struct.pack("<II", 0x04, 0x00);
UDPSock.sendto(halt_data, (msdd_host, msdd_port));


UDPSock.close();

