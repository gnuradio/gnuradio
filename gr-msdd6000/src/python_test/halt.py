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

halt_data = struct.pack("<II", 0x04, 0x00);
halt_data = struct.pack("<II", 0x04, 0x00);

data = halt_data;

UDPSock.sendto(data, (msdd_host, msdd_port));

print "sent"

UDPSock.close();



