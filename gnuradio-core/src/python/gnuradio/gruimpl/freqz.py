#!/usr/bin/env python
#
# Copyright 2005,2007 Free Software Foundation, Inc.
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

# This code lifted from various parts of www.scipy.org -eb 2005-01-24

# Copyright (c) 2001, 2002 Enthought, Inc.
# 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#   a. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#   b. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   c. Neither the name of the Enthought nor the names of its contributors
#      may be used to endorse or promote products derived from this software
#      without specific prior written permission.
# 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# 

__all__ = ['freqz']

import numpy
from numpy import *
Num=numpy

def atleast_1d(*arys):
    """ Force a sequence of arrays to each be at least 1D.

         Description:
            Force an array to be at least 1D.  If an array is 0D, the 
            array is converted to a single row of values.  Otherwise,
            the array is unaltered.
         Arguments:
            *arys -- arrays to be converted to 1 or more dimensional array.
         Returns:
            input array converted to at least 1D array.
    """
    res = []
    for ary in arys:
        ary = asarray(ary)
        if len(ary.shape) == 0: 
            result = numpy.array([ary[0]])
        else:
            result = ary
        res.append(result)
    if len(res) == 1:
        return res[0]
    else:
        return res


def polyval(p,x):
    """Evaluate the polynomial p at x.  If x is a polynomial then composition.

    Description:

      If p is of length N, this function returns the value:
      p[0]*(x**N-1) + p[1]*(x**N-2) + ... + p[N-2]*x + p[N-1]

      x can be a sequence and p(x) will be returned for all elements of x.
      or x can be another polynomial and the composite polynomial p(x) will be
      returned.
    """
    p = asarray(p)
    if isinstance(x,poly1d):
        y = 0
    else:
        x = asarray(x)
        y = numpy.zeros(x.shape,x.typecode())
    for i in range(len(p)):
        y = x * y + p[i]
    return y

class poly1d:
    """A one-dimensional polynomial class.

    p = poly1d([1,2,3]) constructs the polynomial x**2 + 2 x + 3

    p(0.5) evaluates the polynomial at the location
    p.r  is a list of roots
    p.c  is the coefficient array [1,2,3]
    p.order is the polynomial order (after leading zeros in p.c are removed)
    p[k] is the coefficient on the kth power of x (backwards from
         sequencing the coefficient array.

    polynomials can be added, substracted, multplied and divided (returns
         quotient and remainder).
    asarray(p) will also give the coefficient array, so polynomials can
         be used in all functions that accept arrays.
    """
    def __init__(self, c_or_r, r=0):
        if isinstance(c_or_r,poly1d):
            for key in c_or_r.__dict__.keys():
                self.__dict__[key] = c_or_r.__dict__[key]
            return
        if r:
            c_or_r = poly(c_or_r)
        c_or_r = atleast_1d(c_or_r)
        if len(c_or_r.shape) > 1:
            raise ValueError, "Polynomial must be 1d only."
        c_or_r = trim_zeros(c_or_r, trim='f')
        if len(c_or_r) == 0:
            c_or_r = numpy.array([0])
        self.__dict__['coeffs'] = c_or_r
        self.__dict__['order'] = len(c_or_r) - 1

    def __array__(self,t=None):
        if t:
            return asarray(self.coeffs,t)
        else:
            return asarray(self.coeffs)

    def __coerce__(self,other):
        return None
    
    def __repr__(self):
        vals = repr(self.coeffs)
        vals = vals[6:-1]
        return "poly1d(%s)" % vals

    def __len__(self):
        return self.order

    def __str__(self):
        N = self.order
        thestr = "0"
        for k in range(len(self.coeffs)):
            coefstr ='%.4g' % abs(self.coeffs[k])
            if coefstr[-4:] == '0000':
                coefstr = coefstr[:-5]
            power = (N-k)
            if power == 0:
                if coefstr != '0':
                    newstr = '%s' % (coefstr,)
                else:
                    if k == 0:
                        newstr = '0'
                    else:
                        newstr = ''
            elif power == 1:
                if coefstr == '0':
                    newstr = ''
                elif coefstr == '1':
                    newstr = 'x'
                else:                    
                    newstr = '%s x' % (coefstr,)
            else:
                if coefstr == '0':
                    newstr = ''
                elif coefstr == '1':
                    newstr = 'x**%d' % (power,)
                else:                    
                    newstr = '%s x**%d' % (coefstr, power)

            if k > 0:
                if newstr != '':
                    if self.coeffs[k] < 0:
                        thestr = "%s - %s" % (thestr, newstr)
                    else:
                        thestr = "%s + %s" % (thestr, newstr)
            elif (k == 0) and (newstr != '') and (self.coeffs[k] < 0):
                thestr = "-%s" % (newstr,)
            else:
                thestr = newstr
        return _raise_power(thestr)
        

    def __call__(self, val):
        return polyval(self.coeffs, val)

    def __mul__(self, other):
        if isscalar(other):
            return poly1d(self.coeffs * other)
        else:
            other = poly1d(other)
            return poly1d(polymul(self.coeffs, other.coeffs))

    def __rmul__(self, other):
        if isscalar(other):
            return poly1d(other * self.coeffs)
        else:
            other = poly1d(other)
            return poly1d(polymul(self.coeffs, other.coeffs))        
    
    def __add__(self, other):
        other = poly1d(other)
        return poly1d(polyadd(self.coeffs, other.coeffs))        
        
    def __radd__(self, other):
        other = poly1d(other)
        return poly1d(polyadd(self.coeffs, other.coeffs))

    def __pow__(self, val):
        if not isscalar(val) or int(val) != val or val < 0:
            raise ValueError, "Power to non-negative integers only."
        res = [1]
        for k in range(val):
            res = polymul(self.coeffs, res)
        return poly1d(res)

    def __sub__(self, other):
        other = poly1d(other)
        return poly1d(polysub(self.coeffs, other.coeffs))

    def __rsub__(self, other):
        other = poly1d(other)
        return poly1d(polysub(other.coeffs, self.coeffs))

    def __div__(self, other):
        if isscalar(other):
            return poly1d(self.coeffs/other)
        else:
            other = poly1d(other)
            return map(poly1d,polydiv(self.coeffs, other.coeffs))

    def __rdiv__(self, other):
        if isscalar(other):
            return poly1d(other/self.coeffs)
        else:
            other = poly1d(other)
            return map(poly1d,polydiv(other.coeffs, self.coeffs))

    def __setattr__(self, key, val):
        raise ValueError, "Attributes cannot be changed this way."

    def __getattr__(self, key):
        if key in ['r','roots']:
            return roots(self.coeffs)
        elif key in ['c','coef','coefficients']:
            return self.coeffs
        elif key in ['o']:
            return self.order
        else:
            return self.__dict__[key]
        
    def __getitem__(self, val):
        ind = self.order - val
        if val > self.order:
            return 0
        if val < 0:
            return 0
        return self.coeffs[ind]

    def __setitem__(self, key, val):
        ind = self.order - key
        if key < 0:
            raise ValueError, "Does not support negative powers."
        if key > self.order:
            zr = numpy.zeros(key-self.order,self.coeffs.typecode())
            self.__dict__['coeffs'] = numpy.concatenate((zr,self.coeffs))
            self.__dict__['order'] = key
            ind = 0
        self.__dict__['coeffs'][ind] = val
        return

    def integ(self, m=1, k=0):
        return poly1d(polyint(self.coeffs,m=m,k=k))

    def deriv(self, m=1):
        return poly1d(polyder(self.coeffs,m=m))

def freqz(b, a, worN=None, whole=0, plot=None):
    """Compute frequency response of a digital filter.

    Description:

       Given the numerator (b) and denominator (a) of a digital filter compute
       its frequency response.

                  jw               -jw            -jmw
           jw  B(e)    b[0] + b[1]e + .... + b[m]e
        H(e) = ---- = ------------------------------------
                  jw               -jw            -jnw
               A(e)    a[0] + a[2]e + .... + a[n]e             

    Inputs:

       b, a --- the numerator and denominator of a linear filter.
       worN --- If None, then compute at 512 frequencies around the unit circle.
                If a single integer, the compute at that many frequencies.
                Otherwise, compute the response at frequencies given in worN
       whole -- Normally, frequencies are computed from 0 to pi (upper-half of
                unit-circle.  If whole is non-zero compute frequencies from 0
                to 2*pi.

    Outputs: (h,w)

       h -- The frequency response.
       w -- The frequencies at which h was computed.
    """
    b, a = map(atleast_1d, (b,a))
    if whole:
        lastpoint = 2*pi
    else:
        lastpoint = pi
    if worN is None:
        N = 512
        w = Num.arange(0,lastpoint,lastpoint/N)
    elif isinstance(worN, types.IntType):
        N = worN
        w = Num.arange(0,lastpoint,lastpoint/N)
    else:
        w = worN
    w = atleast_1d(w)
    zm1 = exp(-1j*w)
    h = polyval(b[::-1], zm1) / polyval(a[::-1], zm1)
    # if not plot is None:
    #    plot(w, h)
    return h, w
