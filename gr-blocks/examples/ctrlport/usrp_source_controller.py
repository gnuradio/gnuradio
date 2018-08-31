#!/usr/bin/env python

from __future__ import unicode_literals
import sys
import pmt
from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("-H", "--host", default="localhost",
                  help="Hostname to connect to (default=%(default)r)")
parser.add_argument("-p", "--port", type=int, default=9090,
                  help="Port of Controlport instance on host (default=%(default)r)")
parser.add_argument("-a", "--alias", default="gr uhd usrp source0",
                  help="The UHD block's alias to control (default=%(default)r)")
parser.add_argument("command", metavar="COMMAND")
parser.add_argument("value", metavar="VALUE")
args = parser.parse_args()

port = 'command'
cmd = args.command
val = args.value

if(cmd == "tune" or cmd == "time"):
    sys.stderr.write("This application currently does not support the 'tune' or 'time' UHD "
                     "message commands.\n\n")
    sys.exit(1)
if(cmd == "antenna"):
    val = pmt.intern(val)
else:
    val = pmt.from_double(float(val))

argv = [None, args.host, args.port]
radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
radio = radiosys.client

radio.postMessage(args.alias, port, pmt.cons(pmt.intern(cmd), val))
