#!/bin/sh
iverilog -y ../lib/ -y ../../../../usrp/fpga/sdr_lib 	        \
    sounder_tb.v -o sounder_tb 				     && \
./sounder_tb > sounder_tb.out  && \
    grep 'r=0' sounder_tb.out | grep 'c=1' > sounder_tb.out2
    
