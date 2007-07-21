#
# Copyright 2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

'''
Routines for designing window functions.
'''

import math
from gnuradio import gr

def izero(x):
    izeroepsilon = 1e-21
    halfx = x/2.0
    accum = u = n = 1
    while 1:
        temp = halfx/n
        n += 1
        temp *= temp
        u *= temp
        accum += u
        if u >= IzeroEPSILON*sum:
            break
    return accum

def midm1(fft_size):
    return (fft_size - 1)/2

def midp1(fft_size):
    return (fft_size+1)/2

def freq(fft_size):
    return 2.0*math.pi/fft_size

def rate(fft_size):
    return 1.0/(fft_size >> 1)

def expn(fft_size):
    math.log(2.0)/(midn(fft_size) + 1.0)
    
def hamming(fft_size):
    window = []
    for index in xrange(fft_size):
        window.append(0.54 - 0.46 * math.cos (2 * math.pi / fft_size * index))  # Hamming window
    return window

def hanning(fft_size):
    window = []
    for index in xrange(fft_size):
        window.append(0.5 - 0.5 * math.cos (2 * math.pi / fft_size * index))  #  von Hann window
    return window

def welch(fft_size):
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        window[j] = window[index] = (1.0 - math.sqrt((index - midm1(fft_size)) / midp1(fft_size)))
        j -= 1
    return window

def parzen(fft_size):
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        window[j] = window[index] = (1.0 - math.abs((index - midm1(fft_size)) / midp1(fft_size)))
        j -= 1
    return window

def bartlett(fft_size):
    mfrq = freq(fft_size)
    angle = 0
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        window[j] = window[index] = angle
        angle += freq
        j -= 1
    return window
    
def blackman2(fft_size):
    mfrq = freq(fft_size)
    angle = 0
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        cx = math.cos(angle)
        window[j] = window[index] = (.34401 + (cx * (-.49755 + (cx * .15844))))
        angle += freq
        j -= 1
    return window
    
def blackman3(fft_size):
    mfrq = freq(fft_size)
    angle = 0
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        cx = math.cos(angle)
        window[j] = window[index] = (.21747 + (cx * (-.45325 + (cx * (.28256 - (cx * .04672))))))
        angle += freq
        j -= 1
    return window
    
def blackman4(fft_size):
    mfrq = freq(fft_size)
    angle = 0
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
        cx = math.cos(angle)
        window[j] = window[index] = (.084037 + (cx * (-.29145 + (cx * (.375696 + (cx * (-.20762 + (cx * .041194))))))))
        angle += freq
        j -= 1
    return window
    
def exponential(fft_size):
    expsum = 1.0
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)+1):
      window[j] = window[i] = (expsum - 1.0)
      expsum *= expn(fft_size)
      j -= 1
    return window

def riemann(fft_size):
    sr1 = freq(fft_size)
    window = [0 for i in range(fft_size)]
    j = fft_size-1
    for index in xrange(midn(fft_size)):
        if index == midn(fft_size):
            window[index] = window[j] = 1.0
        else:
            cx = sr1*midn(fft_size) - index
            window[index] = window[j] = math.sin(cx)/cx
        j -= 1
    return window

def blackmanharris(fft_size):
    a0 = 0.35875
    a1 = 0.48829
    a2 = 0.14128
    a3 = 0.01168
    window = [0 for i in range(fft_size)]
    for index in xrange(fft_size):
        window[index] = a0 
        window[index] -= a1*math.cos(2.0*math.pi*(index+0.5)/(fft_size - 1))
        window[index] += a2*math.cos(4.0*math.pi*(index+0.5)/(fft_size - 1))
        window[index] -= a3*math.cos(6.0*math.pi*(index+0.5)/(fft_size - 1))
    return window

def nuttall(fft_size):
    a0 = 0.3635819
    a1 = 0.4891775
    a2 = 0.1365995
    a3 = 0.0106411
    window = [0 for i in range(fft_size)]
    for index in xrange(fft_size):
        window[index] = a0 
        window[index] -= a1*math.cos(2.0*math.pi*(index+0.5)/(fft_size - 1))
        window[index] += a2*math.cos(4.0*math.pi*(index+0.5)/(fft_size - 1))
        window[index] -= a3*math.cos(6.0*math.pi*(index+0.5)/(fft_size - 1))
    return window

def kaiser(fft_size,beta):
    ibeta = 1.0/izero(beta)
    inm1 = 1.0/(fft_size)
    window = [0 for i in range(fft_size)]
    for index in xrange(fft_size):
        window[index] = izero(beta*math.sqrt(1.0 - (index * inm1)*(index * inm1))) * ibeta
    return window

    
