/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/* File maxstar.h

   Description: Performs the max* operations (Jacobian logarithm) defined as:
		max*( x, y ) = max( x,y) + log( 1 + exp( - |x-y| ) )

   There are several versions of this function, max_starX, where "X":
      X = 0 For linear approximation to log-MAP
        = 1 For max-log-MAP algorithm (i.e. max*(x,y) = max(x,y) )
        = 2 For Constant-log-MAP algorithm
	    = 3 For log-MAP, correction factor from small nonuniform table and interpolation
        = 4 For log-MAP, correction factor uses C function calls

   Calling syntax:
      output = max_starX( delta1, delta2 )

   Where:
  	  output =	The result of max*(x,y)

   	  delta1 = T] he first argument (i.e. x) of max*(x,y)
	  delta2 = The second argument (i.e. y) of max*(x,y)

   Functions max_star0, max_star1, max_star2, max_star3, and max_star4
   are part of the Iterative Solutions Coded Modulation Library
   The Iterative Solutions Coded Modulation Library is free software;
   you can redistribute it and/or modify it under the terms of 
   the GNU Lesser General Public License as published by the 
   Free Software Foundation; either version 2.1 of the License, 
   or (at your option) any later version.

*/

#ifndef INCLUDED_FECAPI_MAXSTAR_H
#define INCLUDED_FECAPI_MAXSTAR_H

/* values for the jacobian logarithm table (DecoderType=4) */
#define BOUNDARY0    0
#define BOUNDARY1    0.4200
#define BOUNDARY2    0.8500
#define BOUNDARY3    1.3100
#define BOUNDARY4    1.8300
#define BOUNDARY5    2.4100
#define BOUNDARY6    3.1300
#define BOUNDARY7    4.0800
#define BOUNDARY8    5.6000

#define SLOPE0  -0.44788139700522
#define SLOPE1  -0.34691145436176
#define SLOPE2  -0.25432579542705
#define SLOPE3  -0.17326680196715
#define SLOPE4  -0.10822110027877
#define SLOPE5  -0.06002650498009
#define SLOPE6  -0.02739265095522
#define SLOPE7  -0.00860202759280

#define VALUE0   0.68954718055995
#define VALUE1   0.50153699381775
#define VALUE2   0.35256506844219
#define VALUE3   0.23567520254575
#define VALUE4   0.14607646552283
#define VALUE5   0.08360822736113
#define VALUE6   0.04088914377547
#define VALUE7   0.01516612536801

/* values for the constant log-MAP algorithm (DecoderType=3) */
#define CVALUE   0.5
#define TVALUE   1.5

/* values for the linear approximation (DecoderType=1) */
#define TTHRESH  2.508
#define AVALUE  -0.236
#define BVALUE   0.592

/* Values for linear approximation (DecoderType=5) */
#define AJIAN -0.24904163195436
#define TJIAN 2.50681740420944

#endif
