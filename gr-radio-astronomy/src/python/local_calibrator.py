#!/usr/bin/env python
#
# Copyright 2003,2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

import Numeric
import math
import ephem
import time

#
# Simple class for allowing local definition of a calibration function
#  for raw samples coming from the RA detector chain.  Each observatory
#  is different, and rather than hacking up the main code in usrp_ra_receiver
#  we define the appropriate function here.
#
# For example, one could calibrate the output in Janskys, rather than
#  dB.
#
#

def calib_default_total_power(data):
    r = 10.0*math.log10(data)
    return(r)

def calib_numogate_ridge_observatory_total_power(data):

    me = ephem.Observer()

    #
    # PyEphem wants lat/long as strings, rather than floats--took me quite
    #  a long time to figure that out.  If they don't arrive as strings,
    #  the calculations for sidereal time are complete garbage
    #
    me.long = str(-76.043)
    me.lat = str(44.967)

    me.date = ephem.now()
    sidtime = me.sidereal_time()

    foo = time.localtime()
    if not "calib_prefix" in globals():
        pfx = "./"
    else:
        pfx = globals()["calib_prefix"]
    filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
       foo.tm_mon, foo.tm_mday, foo.tm_hour)

    rainbow_file = open (filenamestr+".tpdat","a")
  
    r = (data / 4096.0)
    flt = "%6.3f" % r
    #r = calib_default_total_power(data)
    inter = globals()["calib_decln"]
    rainbow_file.write(str(ephem.hours(sidtime))+" "+flt+" "+str(inter)+"\n")
    rainbow_file.close()
    return(r)

def calib_numogate_ridge_observatory_fft(data,l):

    me = ephem.Observer()

    #
    # PyEphem wants lat/long as strings, rather than floats--took me quite
    #  a long time to figure that out.  If they don't arrive as strings,
    #  the calculations for sidereal time are complete garbage
    #
    me.long = str(-76.043)
    me.lat = str(44.967)

    me.date = ephem.now()
    sidtime = me.sidereal_time()

    foo = time.localtime()
    
    if not "calib_prefix" in globals():
        pfx = "./"
    else:
        pfx = globals()["calib_prefix"]
    filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
       foo.tm_mon, foo.tm_mday, foo.tm_hour)

    now = time.time()

    if not "calib_then" in globals():
        globals()["calib_then"] = now

    delta = (l/1024)*5
		
    if (now - globals()["calib_then"]) >= delta:

        globals()["calib_then"] = now
        rainbow_file = open (filenamestr+".sdat","a")
  
        r = calib_default_fft(data,l)
        inter = globals()["calib_decln"]
        rainbow_file.write("data:"+str(ephem.hours(sidtime))+" "+str(inter)+" "+str(r)+"\n")
        rainbow_file.close()
        return(r)

    return(data)

def calib_default_fft(db,l):
    return(db)

#
# We capture various parameters from the receive chain here, because
#  they can affect the calibration equations.
#
#
def calib_set_gain(gain):
    globals()["calib_gain_setting"] = gain

def calib_set_integ(integ):
    globals()["calib_integ_setting"] = integ

def calib_set_bw(bw):
    globals()["calib_bw_setting"] = bw

def calib_set_freq(freq):
    globals()["calib_freq_setting"] = freq

def calib_set_avg_alpha(alpha):
    globals()["calib_avg_alpha"] = alpha

def calib_set_interesting(inter):
    globals()["calib_is_interesting"] = inter

def calib_set_decln(dec):
    globals()["calib_decln"] = dec

def calib_set_prefix(pfx):
    globals()["calib_prefix"] = pfx
