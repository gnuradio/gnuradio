#!/usr/bin/env python

import sys
import pmt
from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient
from optparse import OptionParser

parser = OptionParser(usage="%prog: [options]")
parser.add_option("-H", "--host", type="string", default="localhost",
                  help="Hostname to connect to (default=%default)")
parser.add_option("-p", "--port", type="int", default=9090,
                  help="Port of Controlport instance on host (default=%default)")
parser.add_option("-a", "--alias", type="string", default="gr uhd usrp source0",
                  help="The UHD block's alias to control (default=%default)")
options, args = parser.parse_args()

if(len(args) < 2):
    sys.stderr.write('Not enough arguments: usrp_source_controller.py [options] <command> <value>\n')
    sys.stderr.write('See the "UHD Interface" section of the manual for available commands.\n\n')
    sys.exit(1)

port = 'command'
alias = options.alias
hostname = options.host
portnum = options.port
cmd = args[0]
val = args[1]

if(cmd == "tune" or cmd == "time"):
    sys.stderr.write("This application currently does not support the 'tune' or 'time' UHD "
                     "message commands.\n\n")
    sys.exit(1)
if(cmd == "antenna"):
    val = pmt.intern(val)
else:
    val = pmt.from_double(float(val))

argv = [None, hostname, portnum]
radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
radio = radiosys.client

radio.postMessage(alias, port, pmt.cons(pmt.intern(cmd), val))
