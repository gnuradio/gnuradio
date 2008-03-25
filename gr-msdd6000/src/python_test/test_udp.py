#!/usr/bin/python

from socket import *
import string
import time
import struct;
import random;

msdd_port = 10001
msdd_host = "10.45.4.43"

my_udp_addr = ("10.45.1.229",10001);

buf = 1024;

#myport = random.randint(1025,65535);
#my_tcp_addr = ("10.45.1.229",myport);
#TCPSock = socket(AF_INET,SOCK_STREAM);
#TCPSock.bind(my_tcp_addr);
#TCPSock.connect((msdd_host,msdd_port));

UDPSock = socket(AF_INET,SOCK_DGRAM);
UDPSock.bind(my_udp_addr);

SETS_STREAM = 0xffffffff;

f_mhz = 2400;
f_hz = 1;
gain = 3;
samples = 512;
decim = 4;
#sets = 16;
sets = SETS_STREAM;
window =  3;
mode = 1;


# construct the 3 different request type packets
fft_data = struct.pack("<IIIIIIIIII", 0x02, 0x20, f_mhz, f_hz, gain,window, samples, decim, mode,sets);
raw_data = struct.pack("<IIIIIIII", 0x01, 0x18, f_mhz, f_hz, gain,samples, decim,sets);
stat_data = struct.pack("!II", 0x0000, 0x0000)

# send appropriate udp request packet
UDPSock.sendto(raw_data, (msdd_host,msdd_port))

#TCPSock.send(raw_data);


print "sent request"


print "waiting for response"
data,addr = UDPSock.recvfrom(buf);

print "got response"
print data,addr


