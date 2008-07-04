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

port = 10000
host = "10.45.4.46"
#host = "10.45.4.41"
myaddr = ('',myport);

buf = 100000;

TCPSock = socket(AF_INET,SOCK_STREAM);
TCPSock.bind(myaddr);
TCPSock.connect((host,port));

#f_mhz = 2647;	# roof ofdm
if(len(sys.argv)!= 3):
	print "usage: %s  fc_ghz   decim_pow2_exponent"%(sys.argv[0]);
	sys.exit(-1);

f_mhz = float(sys.argv[1])*1000;
decim = int(sys.argv[2]);

#f_mhz = 3500;
#f_mhz = 2600;
f_hz = 0;	# offset	
gain = 0; 	
window = 3; 	#0=rect, 1=hanning, 2=hamming, 3=blackman

samples = 65536;
#samples = 16777216;
samples = samples*4; 	#bytes of data we are requesting
samples=samples*2;
#decim = 2;	#0-8   (3 => 2^3 = 8)  
decim = decim+16;	# +16 to use 16bit floats instead of 32 bit floats
mode = 0; 	#0=IQ, 1=MAG, 2=MAGDB
sets = 1;

raw_data = struct.pack("<IIIIIIII", 0x01, 0x18, f_mhz, f_hz, gain, samples, decim, sets);

data = raw_data;

TCPSock.send(data);

print "sent"



count = 0;

total_data = [];

state = 0;

vals = [];
mags = [];
re = [];

sample_count = 0;
IQ_bytes=0;
while(TCPSock):
	if(state==0):
		data = TCPSock.recv(4);
		[opcode] = struct.unpack("<I", data);
		print "Opcode = %d"%(opcode);
		if(opcode==1):
			state = 1;

	elif(state==1):
		data = TCPSock.recv(7*4);
		args = struct.unpack("<IIIIIII", data);
		print ["reply_len", "freq_mhz", "offset_hz", "gain", "sample_bytes", "decim", "sets_remain"];
		print args;
		IQ_bytes = args[0] - 7*4;
		state =2;

	elif(state==2):
		data = TCPSock.recv(4);
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


TCPSock.close();

print "done"
nmags = []
for i in mags:
	if i == 0:
		i=1;
	nmags.append(i);


subplot(2,1,1);
plot(nmags);
#plot(10*log10(nmags));

dlen = len(vals);
fftlen = (dlen-1024)/1024;

fft_data = []
for i in range(1, dlen-1025, 1024):

	t_in = [];
	for ind in range(i, i+1024):
		t_in.append(vals[ind]);

	#tmp = 20*log10(fftshift(fft(t_in)));
	tmp = (fftshift(fft(t_in)));

	if(len(fft_data) == 0):
		for ind in range(0,1024):
			fft_data.append( tmp[ind] );
	else:
		for ind in range(0,1024):
			fft_data[ind] = fft_data[ind] + tmp[ind];

#fft_data = 20*log10(fftshift(fft(vals)));


subplot(2,1,2);
plot(fft_data);
show();

f = open(filename, "w");
for sample in vals:
	binchunk = struct.pack("<ff",float(sample.real), float(sample.imag) );
	f.write(binchunk);
f.close();


