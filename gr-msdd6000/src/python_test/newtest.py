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
msdd_host = "10.45.4.43"

buf = 100000;

my_udp_addr = ('',10001);
my_udp_addr = ('10.45.1.229 ',10001);

UDPSock = socket(AF_INET,SOCK_DGRAM);
UDPSock.bind(my_udp_addr);

#f_mhz = 3500;	
#f_mhz = 3500;	
f_mhz = 100;	
f_hz = 0;	
gain = 0; 	
window = 3; 	#0=rect, 1=hanning, 2=hamming, 3=blackman

samples = 12000;
samples = samples*4; 	#bytes of data we are requesting

decim = 2;	#0-8   (3 => 2^3 = 8)  
decim = decim+16;	# +16 to use 16bit floats instead of 32 bit floats
mode = 0; 	#0=IQ, 1=MAG, 2=MAGDB
#sets = 0;
sets = 0xffffffff;

size_int = 4;
request_len = 6*size_int;	# 6 int items not including the 8 bytes for opcode and length fields
print "request len = %d"%(request_len);

#raw_data = struct.pack("<IIIIIIII", 0x01, 0x18, f_mhz, f_hz, gain, samples, decim, sets);
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

while(True):
	print state;
	if(state==0):
		data = UDPSock.recv(4);
		[opcode] = struct.unpack("<I", data);
		print "Opcode = %d"%(opcode);
		if(opcode==1):

			# if UDP mode and sets_stream requested,
			# we do not get a header reply back, only data
			if(sets == 0):
				state = 1;
			else:
				state = 2;

	elif(state==1):
		data = UDPSock.recv(7*4);
		args = struct.unpack("<IIIIIII", data);
		print ["reply_len", "freq_mhz", "offset_hz", "gain", "sample_bytes", "decim", "sets_remain"];
		print args;
		IQ_bytes = args[0] - 7*4;
		state =2;

	elif(state==2):
		data = UDPSock.recv(4);
		[i,q] = struct.unpack("<hh", data);
		tmp = complex(i,q);

		re.append(i);
		vals.append(tmp);
		mags.append(abs(tmp));


		sample_count = sample_count + 1;
#		print "sample count %d"%(sample_count)	

		IQ_bytes = IQ_bytes - 4;
		if(IQ_bytes < 4):
			print "got all data (total %d)"%(sample_count);
			print "remaining: %d"%(IQ_bytes);
			break;


UDPSock.close();

print "done"
nmags = []
for i in mags:
	if i == 0:
		i=1;
	nmags.append(i);


subplot(2,1,1);
plot(20*log10(nmags));

fft_data = 20*log10(fftshift(fft(vals)));

subplot(2,1,2);
plot(fft_data);
show();

f = open(filename, "w");
for sample in vals:
	binchunk = struct.pack("<ff",float(sample.real), float(sample.imag) );
	f.write(binchunk);
f.close();


