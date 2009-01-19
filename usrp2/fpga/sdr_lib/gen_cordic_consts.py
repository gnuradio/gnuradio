#!/usr/bin/env python

import math

zwidth = 24

for i in range(24):
    c = math.atan (1.0/(2**i)) / (2 * math.pi) * (1 << zwidth)
    print "localparam c%02d = %d'd%d;" % (i, zwidth, round (c))
    
