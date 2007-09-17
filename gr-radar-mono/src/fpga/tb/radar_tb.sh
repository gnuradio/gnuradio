#!/bin/sh
iverilog \
    -D SIMULATION \
    -y ../lib/ \
    -y ../../../../usrp/fpga/sdr_lib \
    -y ../models/ \
    radar_tb.v -o radar_tb && ./radar_tb > radar_tb.out
