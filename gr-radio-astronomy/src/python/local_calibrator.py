#!/usr/bin/env python
#
# Copyright 2003,2004,2005 Free Software Foundation, Inc.
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

import Numeric
import math
import ephem
import time

#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NO LONGER USED
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#


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
    me.long = globals()["calib_long"]
    me.lat = globals()["calib_lat"]

    me.date = ephem.now()
    sidtime = me.sidereal_time()

    foo = time.localtime()
    if not "calib_prefix" in globals():
        pfx = "./"
    else:
        pfx = globals()["calib_prefix"]
    filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
       foo.tm_mon, foo.tm_mday, foo.tm_hour)

    numogate_file = open (filenamestr+".tpdat","a")
  
    r = (data / 409.6)
    flt = "%6.3f" % r
    #r = calib_default_total_power(data)
    inter = globals()["calib_decln"]
    integ = globals()["calib_integ_setting"]
    fc = globals()["calib_freq_setting"]
    fc = fc / 1000000
    bw = globals()["calib_bw_setting"]
    bw = bw / 1000000
    ga = globals()["calib_gain_setting"]

    now = time.time()

    if not "calib_then_tpdat" in globals():
        globals()["calib_then_tpdat"] = now

    if (now - globals()["calib_then_tpdat"]) >= 20:
        globals()["calib_then_tpdat"] = now
    
        numogate_file.write(str(ephem.hours(sidtime))+" "+flt+" Dn="+str(inter)+",")
        numogate_file.write("Ti="+str(integ)+",Fc="+str(fc)+",Bw="+str(bw))
        numogate_file.write(",Ga="+str(ga)+"\n")
    else:
        numogate_file.write(str(ephem.hours(sidtime))+" "+flt+"\n")

    numogate_file.close()
    return(r)

def calib_numogate_ridge_observatory_fft(data,l):

    me = ephem.Observer()

    #
    # PyEphem wants lat/long as strings, rather than floats--took me quite
    #  a long time to figure that out.  If they don't arrive as strings,
    #  the calculations for sidereal time are complete garbage
    #
    me.long = globals()["calib_long"]
    me.lat = globals()["calib_lat"]

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
        numogate_file = open (filenamestr+".sdat","a")
  
        r = data
        inter = globals()["calib_decln"]
        fc = globals()["calib_freq_setting"]
        fc = fc / 1000000
        bw = globals()["calib_bw_setting"]
        bw = bw / 1000000
        av = globals()["calib_avg_alpha"]
        numogate_file.write("data:"+str(ephem.hours(sidtime))+" Dn="+str(inter)+",Fc="+str(fc)+",Bw="+str(bw)+",Av="+str(av))
        numogate_file.write(" "+str(r)+"\n")
        numogate_file.close()
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
    globals()["calib_then_tpdat"] = time.time() - 50

def calib_set_integ(integ):
    globals()["calib_integ_setting"] = integ
    globals()["calib_then_tpdat"] = time.time() - 50

def calib_set_bw(bw):
    globals()["calib_bw_setting"] = bw
    globals()["calib_then_tpdat"] = time.time() - 50

def calib_set_freq(freq):
    globals()["calib_freq_setting"] = freq
    globals()["calib_then_tpdat"] = time.time() - 50

def calib_set_avg_alpha(alpha):
    globals()["calib_avg_alpha"] = alpha

def calib_set_interesting(inter):
    globals()["calib_is_interesting"] = inter

def calib_set_decln(dec):
    globals()["calib_decln"] = dec
    globals()["calib_then_tpdat"] = time.time() - 50

def calib_set_prefix(pfx):
    globals()["calib_prefix"] = pfx

def calib_set_long(long):
    globals()["calib_long"] = long

def calib_set_lat(lat):
    globals()["calib_lat"] = lat
