#!/usr/bin/python

from socket import *
import string
import time
import struct;


port = 10001
host = "10.45.4.43"

myaddr = ("10.45.1.229",10000);

buf = 1024;

UDPSock = socket(AF_INET,SOCK_DGRAM);
UDPSock.bind(myaddr);

f_mhz = 2400;
f_hz = 1;
gain = 3;
samples = 512;
decim = 2;
sets = 16;


req_data = struct.pack("<IIIIIIIIII", 0x0001, 0x0002, f_mhz, f_hz, gain, samples, decim, sets);
data = struct.pack("<II", 0x0000, 0x0000);



UDPSock.sendto(stat_data, (host,port))


print "sent"

data,addr = UDPSock.recvfrom(buf);
print "got response"


