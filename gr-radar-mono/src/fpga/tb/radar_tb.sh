#!/bin/sh
iverilog -y ../lib/ -y ../../../../usrp/fpga/sdr_lib 	        \
    radar_tb.v -o radar_tb && ./radar_tb > radar_tb.out
