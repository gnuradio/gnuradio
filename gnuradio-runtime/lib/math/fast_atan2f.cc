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

  #define TAN_MAP_RES 0.003921569 /* (smallest non-zero value in table) */
  #define RAD_PER_DEG 0.017453293
  #define TAN_MAP_SIZE 255

  /* arctangents from 0 to pi/4 radians */
  static float
  fast_atan_table[257] = {
    0.000000e+00, 3.921549e-03, 7.842976e-03, 1.176416e-02,
    1.568499e-02, 1.960533e-02, 2.352507e-02, 2.744409e-02,
    3.136226e-02, 3.527947e-02, 3.919560e-02, 4.311053e-02,
    4.702413e-02, 5.093629e-02, 5.484690e-02, 5.875582e-02,
    6.266295e-02, 6.656816e-02, 7.047134e-02, 7.437238e-02,
    7.827114e-02, 8.216752e-02, 8.606141e-02, 8.995267e-02,
    9.384121e-02, 9.772691e-02, 1.016096e-01, 1.054893e-01,
    1.093658e-01, 1.132390e-01, 1.171087e-01, 1.209750e-01,
    1.248376e-01, 1.286965e-01, 1.325515e-01, 1.364026e-01,
    1.402496e-01, 1.440924e-01, 1.479310e-01, 1.517652e-01,
    1.555948e-01, 1.594199e-01, 1.632403e-01, 1.670559e-01,
    1.708665e-01, 1.746722e-01, 1.784728e-01, 1.822681e-01,
    1.860582e-01, 1.898428e-01, 1.936220e-01, 1.973956e-01,
    2.011634e-01, 2.049255e-01, 2.086818e-01, 2.124320e-01,
    2.161762e-01, 2.199143e-01, 2.236461e-01, 2.273716e-01,
    2.310907e-01, 2.348033e-01, 2.385093e-01, 2.422086e-01,
    2.459012e-01, 2.495869e-01, 2.532658e-01, 2.569376e-01,
    2.606024e-01, 2.642600e-01, 2.679104e-01, 2.715535e-01,
    2.751892e-01, 2.788175e-01, 2.824383e-01, 2.860514e-01,
    2.896569e-01, 2.932547e-01, 2.968447e-01, 3.004268e-01,
    3.040009e-01, 3.075671e-01, 3.111252e-01, 3.146752e-01,
    3.182170e-01, 3.217506e-01, 3.252758e-01, 3.287927e-01,
    3.323012e-01, 3.358012e-01, 3.392926e-01, 3.427755e-01,
    3.462497e-01, 3.497153e-01, 3.531721e-01, 3.566201e-01,
    3.600593e-01, 3.634896e-01, 3.669110e-01, 3.703234e-01,
    3.737268e-01, 3.771211e-01, 3.805064e-01, 3.838825e-01,
    3.872494e-01, 3.906070e-01, 3.939555e-01, 3.972946e-01,
    4.006244e-01, 4.039448e-01, 4.072558e-01, 4.105574e-01,
    4.138496e-01, 4.171322e-01, 4.204054e-01, 4.236689e-01,
    4.269229e-01, 4.301673e-01, 4.334021e-01, 4.366272e-01,
    4.398426e-01, 4.430483e-01, 4.462443e-01, 4.494306e-01,
    4.526070e-01, 4.557738e-01, 4.589307e-01, 4.620778e-01,
    4.652150e-01, 4.683424e-01, 4.714600e-01, 4.745676e-01,
    4.776654e-01, 4.807532e-01, 4.838312e-01, 4.868992e-01,
    4.899573e-01, 4.930055e-01, 4.960437e-01, 4.990719e-01,
    5.020902e-01, 5.050985e-01, 5.080968e-01, 5.110852e-01,
    5.140636e-01, 5.170320e-01, 5.199904e-01, 5.229388e-01,
    5.258772e-01, 5.288056e-01, 5.317241e-01, 5.346325e-01,
    5.375310e-01, 5.404195e-01, 5.432980e-01, 5.461666e-01,
    5.490251e-01, 5.518738e-01, 5.547124e-01, 5.575411e-01,
    5.603599e-01, 5.631687e-01, 5.659676e-01, 5.687566e-01,
    5.715357e-01, 5.743048e-01, 5.770641e-01, 5.798135e-01,
    5.825531e-01, 5.852828e-01, 5.880026e-01, 5.907126e-01,
    5.934128e-01, 5.961032e-01, 5.987839e-01, 6.014547e-01,
    6.041158e-01, 6.067672e-01, 6.094088e-01, 6.120407e-01,
    6.146630e-01, 6.172755e-01, 6.198784e-01, 6.224717e-01,
    6.250554e-01, 6.276294e-01, 6.301939e-01, 6.327488e-01,
    6.352942e-01, 6.378301e-01, 6.403565e-01, 6.428734e-01,
    6.453808e-01, 6.478788e-01, 6.503674e-01, 6.528466e-01,
    6.553165e-01, 6.577770e-01, 6.602282e-01, 6.626701e-01,
    6.651027e-01, 6.675261e-01, 6.699402e-01, 6.723452e-01,
    6.747409e-01, 6.771276e-01, 6.795051e-01, 6.818735e-01,
    6.842328e-01, 6.865831e-01, 6.889244e-01, 6.912567e-01,
    6.935800e-01, 6.958943e-01, 6.981998e-01, 7.004964e-01,
    7.027841e-01, 7.050630e-01, 7.073330e-01, 7.095943e-01,
    7.118469e-01, 7.140907e-01, 7.163258e-01, 7.185523e-01,
    7.207701e-01, 7.229794e-01, 7.251800e-01, 7.273721e-01,
    7.295557e-01, 7.317307e-01, 7.338974e-01, 7.360555e-01,
    7.382053e-01, 7.403467e-01, 7.424797e-01, 7.446045e-01,
    7.467209e-01, 7.488291e-01, 7.509291e-01, 7.530208e-01,
    7.551044e-01, 7.571798e-01, 7.592472e-01, 7.613064e-01,
    7.633576e-01, 7.654008e-01, 7.674360e-01, 7.694633e-01,
    7.714826e-01, 7.734940e-01, 7.754975e-01, 7.774932e-01,
    7.794811e-01, 7.814612e-01, 7.834335e-01, 7.853982e-01,
    7.853982e-01
  };


  /*****************************************************************************
   Function: Arc tangent

   Syntax: angle = fast_atan2(y, x);
   float y y component of input vector
   float x x component of input vector
   float angle angle of vector (x, y) in radians

   Description: This function calculates the angle of the vector (x,y)
   based on a table lookup and linear interpolation. The table uses a
   256 point table covering -45 to +45 degrees and uses symmetry to
   determine the final angle value in the range of -180 to 180
   degrees. Note that this function uses the small angle approximation
   for values close to zero. This routine calculates the arc tangent
   with an average error of +/- 3.56e-5 degrees (6.21e-7 radians).
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
      index = ((int)alpha) & 0xff;
      alpha -= (float)index;
      /* determine base angle based on quadrant and */
      /* add or subtract table value from base angle based on quadrant */
      base_angle  =  fast_atan_table[index];
      base_angle += (fast_atan_table[index + 1] - fast_atan_table[index]) * alpha;
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
