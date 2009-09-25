/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_VRT_TYPES_H
#define INCLUDED_VRT_TYPES_H

#include <stdint.h>

/* macros for dealing with fixed point numbers */
#define _FXPT_C(_type, _x, _rp) ((_type)((_x)*(1ll << _rp)))
#define _FXPT_TO_INT(_x, _one) (((_x) + ((_one)/2))/(_one))
#define _FXPT_TO_DOUBLE(_x, _one) ((double)(_x) * (1.0/(_one)))

/***********************************************************************
 * The VRT Altitude Type (meters)
 **********************************************************************/
typedef int32_t vrt_altitude_t;
#define VRT_ALTITUDE_RP 5
#define VRT_ALTITUDE_C(_x) _FXPT_C(vrt_altitude_t, _x, VRT_ALTITUDE_RP)

static inline vrt_altitude_t
double_to_vrt_altitude(double num){
  return VRT_ALTITUDE_C(num);
}

static inline int32_t
vrt_altitude_round_to_int(vrt_altitude_t fx){
  return _FXPT_TO_INT(fx, VRT_ALTITUDE_C(1));
}

static inline double
vrt_altitude_to_double(vrt_altitude_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_ALTITUDE_C(1));
}

/***********************************************************************
 * The VRT Geolocation Angle Type (degrees)
 **********************************************************************/
typedef int32_t vrt_geo_angle_t;
#define VRT_GEO_ANGLE_RP 22
#define VRT_GEO_ANGLE_C(_x) _FXPT_C(vrt_geo_angle_t, _x, VRT_GEO_ANGLE_RP)

static inline vrt_geo_angle_t
double_to_vrt_geo_angle(double num){
  return VRT_GEO_ANGLE_C(num);
}

static inline int16_t
vrt_geo_angle_round_to_int(vrt_geo_angle_t fx){
  return _FXPT_TO_INT(fx, VRT_GEO_ANGLE_C(1));
}

static inline double
vrt_geo_angle_to_double(vrt_geo_angle_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_GEO_ANGLE_C(1));
}

/***********************************************************************
 * The VRT Frequency Type (Hz)
 **********************************************************************/
typedef int64_t vrt_freq_t;
#define VRT_FREQ_RP 20
#define VRT_FREQ_C(_x) _FXPT_C(vrt_altitude_t, _x, VRT_FREQ_RP)

static inline vrt_freq_t
double_to_vrt_freq(double num){
  return VRT_FREQ_C(num);
}

static inline int64_t
vrt_freq_round_to_int(vrt_freq_t fx){
  return _FXPT_TO_INT(fx, VRT_FREQ_C(1));
}

static inline double
vrt_freq_to_double(vrt_freq_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_FREQ_C(1));
}

/***********************************************************************
 * The VRT Gain Type (dB)
 **********************************************************************/
typedef int16_t vrt_gain_t;
#define VRT_GAIN_RP 7
#define VRT_GAIN_C(_x) _FXPT_C(vrt_gain_t, _x, VRT_GAIN_RP)

static inline vrt_gain_t
double_to_vrt_gain(double num){
  return VRT_GAIN_C(num);
}

static inline int16_t
vrt_gain_round_to_int(vrt_gain_t fx){
  return _FXPT_TO_INT(fx, VRT_GAIN_C(1));
}

static inline double
vrt_gain_to_double(vrt_gain_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_GAIN_C(1));
}

/***********************************************************************
 * The VRT Temperature Type (Celcius)
 **********************************************************************/
typedef int16_t vrt_temp_t;
#define VRT_TEMP_RP 6
#define VRT_TEMP_C(_x) _FXPT_C(vrt_temp_t, _x, VRT_TEMP_RP)

static inline vrt_temp_t
double_to_vrt_temp(double num){
  return VRT_TEMP_C(num);
}

static inline int16_t
vrt_temp_round_to_int(vrt_temp_t fx){
  return _FXPT_TO_INT(fx, VRT_TEMP_C(1));
}

static inline double
vrt_temp_to_double(vrt_temp_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_TEMP_C(1));
}

#endif /* INCLUDED_VRT_TYPES_H */
