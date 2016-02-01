#!/usr/bin/env python

import sys
import pmt
from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient

args = sys.argv
if(len(args) < 4):
    sys.stderr.write('Not enough arguments: simple_copy_controller.py <host> <port> [true|false]\n\n')
    sys.exit(1)

hostname = args[1]
portnum = int(args[2])
msg = args[3].lower()
argv = [None, hostname, portnum]
radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
radio = radiosys.client

if(msg == 'true'):
    radio.postMessage('copy0', 'en', pmt.PMT_T)
elif(msg == 'false'):
    radio.postMessage('copy0', 'en', pmt.PMT_F)
else:
    sys.stderr.write('Unrecognized message: must be true or false.\n\n')
    sys.exit(1)
