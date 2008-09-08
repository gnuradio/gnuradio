#!/usr/bin/env python

speeds = (9600, 19200, 38400, 57600, 115200, 230400)

master_clk = 100e6
wb_clk = master_clk / 2

def divisor(speed):
    div0 = wb_clk // (speed * 16)
    div1 = div0 + 1
    actual0 = actual_speed(div0)
    actual1 = actual_speed(div1)
    if abs(actual0 - speed) < abs(actual1 - speed):
        return div0
    else:
        return div1

def actual_speed(divisor):
    return (wb_clk // divisor) / 16

def doit(speed):
    div = divisor(speed)
    actual = actual_speed(div)
    rel_error = (actual - speed) / speed
    print "target: %6d  divisor: %6d  actual: %11.4f  %6.3f%%" % (speed, div, actual, rel_error*100)

def main():
    print "wb_clk = %f" % (wb_clk,)
    for s in speeds:
        doit(s)

if __name__ == '__main__':
    main()
    
