#!/usr/bin/env python

import sys
import pmt
from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient

args = sys.argv
if(len(args) < 4):
    sys.stderr.write('Not enough arguments: usrp_source_controller.py <host> <port> <command> <value>\n')
    sys.stderr.write('See the "UHD Interface" section of the manual for available commands.\n\n')
    sys.exit(1)

alias = 'usrp_source0'
port = 'command'

hostname = args[1]
portnum = int(args[2])
cmd = args[3].lower()

if(cmd == "antenna"):
    val = pmt.intern(args[4])
else:
    val = pmt.from_double(float(args[4]))

argv = [None, hostname, portnum]
radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
radio = radiosys.client

radio.postMessage(alias, port, pmt.cons(pmt.intern(cmd), val))
