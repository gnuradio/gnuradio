#!/usr/bin/env python

import math

zwidth = 16

for i in range(17):
    c = math.atan (1.0/(2**i)) / (2 * math.pi) * (1 << zwidth)
    print "`define c%02d %d'd%d" % (i, zwidth, round (c))
    
