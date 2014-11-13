/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

#include <gnuradio/math.h>         // declaration is in here
#include <cmath>

namespace gr {

  /***************************************************************************/
  /* Constant definitions */
  /***************************************************************************/

  #define TAN_MAP_RES 0.00390625	/* TAN_MAP_RES = 1.0 / 256.0 */
  #define RAD_PER_DEG 0.017453293
  #define TAN_MAP_SIZE 256

  /* arctangents from 0 to pi/4 radians */
  static float
  fast_atan_table[258] = {
	0.000000e00, 3.906230e-03, 7.812341e-03, 1.171821e-02, 
	1.562373e-02, 1.952877e-02, 2.343321e-02, 2.733694e-02, 
	3.123983e-02, 3.514178e-02, 3.904265e-02, 4.294233e-02, 
	4.684071e-02, 5.073767e-02, 5.463308e-02, 5.852683e-02, 
	6.241881e-02, 6.630889e-02, 7.019697e-02, 7.408292e-02, 
	7.796663e-02, 8.184799e-02, 8.572688e-02, 8.960318e-02, 
	9.347678e-02, 9.734757e-02, 1.012154e-01, 1.050803e-01, 
	1.089420e-01, 1.128004e-01, 1.166554e-01, 1.205070e-01, 
	1.243550e-01, 1.281993e-01, 1.320398e-01, 1.358763e-01, 
	1.397089e-01, 1.435373e-01, 1.473615e-01, 1.511813e-01, 
	1.549967e-01, 1.588076e-01, 1.626138e-01, 1.664153e-01, 
	1.702119e-01, 1.740036e-01, 1.777902e-01, 1.815717e-01, 
	1.853479e-01, 1.891188e-01, 1.928843e-01, 1.966442e-01, 
	2.003986e-01, 2.041471e-01, 2.078899e-01, 2.116268e-01, 
	2.153577e-01, 2.190825e-01, 2.228012e-01, 2.265135e-01, 
	2.302196e-01, 2.339192e-01, 2.376123e-01, 2.412988e-01, 
	2.449787e-01, 2.486517e-01, 2.523180e-01, 2.559773e-01, 
	2.596296e-01, 2.632749e-01, 2.669130e-01, 2.705439e-01, 
	2.741675e-01, 2.777837e-01, 2.813924e-01, 2.849937e-01, 
	2.885874e-01, 2.921734e-01, 2.957517e-01, 2.993222e-01, 
	3.028849e-01, 3.064396e-01, 3.099864e-01, 3.135251e-01, 
	3.170558e-01, 3.205782e-01, 3.240925e-01, 3.275984e-01, 
	3.310961e-01, 3.345853e-01, 3.380661e-01, 3.415384e-01, 
	3.450022e-01, 3.484573e-01, 3.519038e-01, 3.553416e-01, 
	3.587707e-01, 3.621909e-01, 3.656023e-01, 3.690049e-01, 
	3.723984e-01, 3.757831e-01, 3.791587e-01, 3.825252e-01, 
	3.858827e-01, 3.892310e-01, 3.925701e-01, 3.959001e-01, 
	3.992208e-01, 4.025322e-01, 4.058343e-01, 4.091271e-01, 
	4.124104e-01, 4.156844e-01, 4.189490e-01, 4.222040e-01, 
	4.254496e-01, 4.286857e-01, 4.319122e-01, 4.351292e-01, 
	4.383366e-01, 4.415343e-01, 4.447224e-01, 4.479009e-01, 
	4.510697e-01, 4.542287e-01, 4.573781e-01, 4.605177e-01, 
	4.636476e-01, 4.667677e-01, 4.698781e-01, 4.729786e-01, 
	4.760693e-01, 4.791502e-01, 4.822213e-01, 4.852826e-01, 
	4.883340e-01, 4.913755e-01, 4.944071e-01, 4.974289e-01, 
	5.004408e-01, 5.034428e-01, 5.064349e-01, 5.094171e-01, 
	5.123895e-01, 5.153519e-01, 5.183044e-01, 5.212470e-01, 
	5.241796e-01, 5.271024e-01, 5.300153e-01, 5.329182e-01, 
	5.358112e-01, 5.386944e-01, 5.415676e-01, 5.444309e-01, 
	5.472844e-01, 5.501279e-01, 5.529616e-01, 5.557854e-01, 
	5.585993e-01, 5.614034e-01, 5.641976e-01, 5.669819e-01, 
	5.697565e-01, 5.725211e-01, 5.752760e-01, 5.780211e-01, 
	5.807564e-01, 5.834818e-01, 5.861976e-01, 5.889035e-01, 
	5.915997e-01, 5.942862e-01, 5.969629e-01, 5.996300e-01, 
	6.022873e-01, 6.049350e-01, 6.075731e-01, 6.102014e-01, 
	6.128202e-01, 6.154294e-01, 6.180289e-01, 6.206189e-01, 
	6.231993e-01, 6.257702e-01, 6.283316e-01, 6.308835e-01, 
	6.334259e-01, 6.359588e-01, 6.384823e-01, 6.409964e-01, 
	6.435011e-01, 6.459964e-01, 6.484824e-01, 6.509590e-01, 
	6.534263e-01, 6.558844e-01, 6.583331e-01, 6.607727e-01, 
	6.632030e-01, 6.656241e-01, 6.680361e-01, 6.704389e-01, 
	6.728325e-01, 6.752171e-01, 6.775926e-01, 6.799591e-01, 
	6.823166e-01, 6.846650e-01, 6.870045e-01, 6.893350e-01, 
	6.916566e-01, 6.939693e-01, 6.962732e-01, 6.985682e-01, 
	7.008544e-01, 7.031318e-01, 7.054005e-01, 7.076604e-01, 
	7.099116e-01, 7.121542e-01, 7.143881e-01, 7.166133e-01, 
	7.188300e-01, 7.210381e-01, 7.232377e-01, 7.254287e-01, 
	7.276113e-01, 7.297855e-01, 7.319512e-01, 7.341085e-01, 
	7.362574e-01, 7.383980e-01, 7.405303e-01, 7.426544e-01, 
	7.447701e-01, 7.468777e-01, 7.489770e-01, 7.510682e-01, 
	7.531513e-01, 7.552262e-01, 7.572931e-01, 7.593520e-01, 
	7.614028e-01, 7.634456e-01, 7.654805e-01, 7.675074e-01, 
	7.695265e-01, 7.715377e-01, 7.735410e-01, 7.755366e-01, 
	7.775243e-01, 7.795043e-01, 7.814766e-01, 7.834412e-01, 
	7.853982e-01, 7.853982e-01
   };


  /*****************************************************************************
   Function: Arc tangent

   Syntax: angle = fast_atan2(y, x);
   float y y component of input vector
   float x x component of input vector
   float angle angle of vector (x, y) in radians

   Description: This function calculates the angle of the vector (x,y)
   based on a table lookup and linear interpolation. The table uses a
   256 point table covering -45 to +45 degrees and uses symetry to
   determine the final angle value in the range of -180 to 180
   degrees. Note that this function uses the small angle approximation
   for values close to zero. This routine calculates the arc tangent
   with an average error of +/- 0.045 degrees.
  *****************************************************************************/

  float
  fast_atan2f(float y, float x)
  {
    float x_abs, y_abs, z;
    float alpha, angle, base_angle;
    int index;

    /* normalize to +/- 45 degree range */
    y_abs = fabsf(y);
    x_abs = fabsf(x);
    /* don't divide by zero! */
    if(!((y_abs > 0.0f) || (x_abs > 0.0f)))
      return 0.0;

    //z = (y_abs < x_abs ? y_abs / x_abs : x_abs / y_abs);
    if(y_abs < x_abs)
      z = y_abs / x_abs;
    else
      z = x_abs / y_abs;

    /* when ratio approaches the table resolution, the angle is */
    /* best approximated with the argument itself... */
    if(z < TAN_MAP_RES)
      base_angle = z;
    else {
      /* find index and interpolation value */
      alpha = z * (float)TAN_MAP_SIZE;
      index = (int)alpha;
      alpha -= (float)index;
      /* determine base angle based on quadrant and */
      /* add or subtract table value from base angle based on quadrant */
      base_angle = fast_atan_table[index];
      base_angle +=
        (fast_atan_table[index + 1] - fast_atan_table[index]) * alpha;
    }

    if(x_abs > y_abs) { /* -45 -> 45 or 135 -> 225 */
      if(x >= 0.0) { /* -45 -> 45 */
        if(y >= 0.0)
          angle = base_angle; /* 0 -> 45, angle OK */
        else
          angle = -base_angle; /* -45 -> 0, angle = -angle */
      }
      else { /* 135 -> 180 or 180 -> -135 */
        angle = 3.14159265358979323846;
        if(y >= 0.0)
          angle -= base_angle; /* 135 -> 180, angle = 180 - angle */
        else
          angle = base_angle - angle; /* 180 -> -135, angle = angle - 180 */
      }
    }
    else { /* 45 -> 135 or -135 -> -45 */
      if(y >= 0.0) { /* 45 -> 135 */
        angle = 1.57079632679489661923;
        if(x >= 0.0)
          angle -= base_angle; /* 45 -> 90, angle = 90 - angle */
        else
          angle += base_angle; /* 90 -> 135, angle = 90 + angle */
      }
      else { /* -135 -> -45 */
        angle = -1.57079632679489661923;
        if(x >= 0.0)
          angle += base_angle; /* -90 -> -45, angle = -90 + angle */
        else
          angle -= base_angle; /* -135 -> -90, angle = -90 - angle */
      }
    }

  #ifdef ZERO_TO_TWOPI
    if (angle < 0)
      return (angle + TWOPI);
    else
      return (angle);
  #else
    return (angle);
  #endif
  }

} /* namespace gr */
