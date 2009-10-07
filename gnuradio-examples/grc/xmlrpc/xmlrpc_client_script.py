#!/usr/bin/env python

import time
import random
import xmlrpclib

#create server object
s = xmlrpclib.Server("http://localhost:1234")

#randomly change parameters of the sinusoid
for i in range(10):
	#generate random values
	new_freq = random.uniform(0, 5000)
	new_ampl = random.uniform(0, 2)
	new_offset = random.uniform(-1, 1)
	#set new values 
	time.sleep(1)
	s.set_freq(new_freq)
	time.sleep(1)
	s.set_ampl(new_ampl)
	time.sleep(1)
	s.set_offset(new_offset)

