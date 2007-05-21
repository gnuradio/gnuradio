#!/bin/sh
iverilog -y ../lib/ -y ../../../../usrp/fpga/sdr_lib 	        \
    sounder_tb.v -o sounder_tb 				     && \
./sounder_tb                                       > sounder_tb.out  && \
grep 'rst=0'         sounder_tb.out | grep 'clk=1' > sounder_tb.out2 && \
grep 'tx_strobe=1'   sounder_tb.out2               > sounder_tb.out3
    
