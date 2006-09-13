/*
 Copyright 2006 Johnathan Corgan.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2
 as published by the Free Software Foundation.
 
 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with GNU Radio; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street,
 Boston, MA 02110-1301, USA.
*/

#ifndef __UTIL_H__
#define __UTIL_H__

// System level includes
#include <cmath>

inline double limit(double x, double lower, double upper)
{
    if (x < lower)
        return lower;
    else if (x > upper)
        return upper;
    else
        return x;
}

inline int limit(int x, int lower, int upper)
{
    if (x < lower)
        return lower;
    else if (x > upper)
        return upper;
    else
        return x;
}

inline double degree_normalize(double degrees)
{
    if (degrees >= 360.0)
        return degrees - 360.0;
    else if (degrees < 0.0)
        return degrees + 360.0;
    else
        return degrees;
}

inline int degree_normalize(int degrees)
{
    if (degrees >= 360)
        return degrees - 360;
    else if (degrees < 0)
        return degrees + 360;
    else
        return degrees;
}

inline double to_radians(double degrees)
{
    return degrees/180.0*M_PI;
}

inline double to_degrees(double radians)
{
    return radians/M_PI*180.0;
}

#define LOGFUNCTION wxLogDebug("%s", __PRETTY_FUNCTION__)
#define LOGFUNCTIONENTRY wxLogDebug("%s: entered", __PRETTY_FUNCTION__)
#define LOGFUNCTIONEXIT wxLogDebug("%s: exited", __PRETTY_FUNCTION__)

#endif // __UTIL_H__
