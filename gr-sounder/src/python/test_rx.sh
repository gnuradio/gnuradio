#!/bin/sh

usrp_sounder.py -r -n5000 -d12
hexdump output.dat >output.hex
