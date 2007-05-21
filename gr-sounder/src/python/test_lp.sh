#!/bin/sh

usrp_sounder.py -r -l -t -n5000 -d12
hexdump output.dat >output.hex
