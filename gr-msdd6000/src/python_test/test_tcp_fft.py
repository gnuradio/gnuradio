#!/usr/bin/python

from socket import *
import string
import time
import struct;
import random;

myport = random.randint(1025,65535);

port = 10000
host = "10.45.4.43"
myaddr = ("10.45.1.229",myport);

buf = 100000;

TCPSock = socket(AF_INET,SOCK_STREAM);
#TCPSock = socket(AF_INET,SOCK_DGRAM);
TCPSock.bind(myaddr);
TCPSock.connect((host,port));

f_mhz = 2400;	
f_hz = 0;	
gain = 2; 	
window = 3; 	#0=rect, 1=hanning, 2=hamming, 3=blackman
#samples = 0xffffffff; 	#8-15	fft:(returns 2^number[8-15]) raw:(returns number)
samples = 2; 	#8-15	fft:(returns 2^number[8-15]) raw:(returns number)
decim = 2; 	#0-8  
#decim = decim+16;	# +16 to use 16bit instead of 32 bit 
mode = 1; 	#0=IQ, 1=MAG, 2=MAGDB
sets = 0xffffffff;	
#sets = 1;

fft_data = struct.pack("<IIIIIIIIII", 0x02, 0x20, f_mhz, f_hz, gain,window, samples, decim, mode,sets);
raw_data = struct.pack("<IIIIIIII", 0x01, 0x18, f_mhz, f_hz, gain,samples, decim,sets);
stat_data = struct.pack("!II", 0x0000, 0x0000)

data = raw_data;

#TCPSock.sendto(data, (host,port))
TCPSock.send(data);

print "sent"



count = 0;
while(1):
	data,addr = TCPSock.recvfrom(buf);
	
	print "got response"
	
	print "Data length: %d bytes."%(len(data));
	if(len(data)==12):
		a,b,c = struct.unpack("!III",data);
		print "%x,%x,%x"%(a,b,c);

	datavector = [];

	for d in data:
		a = struct.unpack("<b",d);
		datavector.append(a);

	print datavector;
		
	count = count + 1;
	
	if(count > 1):
		sets = 3;
		raw_data_2 = struct.pack("<IIIIIIII", 0x01, 0x18, f_mhz, f_hz, gain,samples, decim,sets);
		TCPSock.send(raw_data_2);
		
		
		
TCPSock.close();



