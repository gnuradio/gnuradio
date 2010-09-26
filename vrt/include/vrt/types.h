/* -*- c -*- */
/*
 * Copyright 2009,2010 Free Software Foundation, Inc.
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* macros for dealing with fixed point numbers */
#define _FXPT_C(_type, _x, _rp) ((_type)((_x)*(1LL << _rp)))
#define _FXPT_TO_INT(_x, _one) (((_x) + ((_one)/2))/(_one))
#define _FXPT_TO_DOUBLE(_x, _one) ((double)(_x) * (1.0/(_one)))

/*
 * The VRT Distance type (meters)
 */
typedef int32_t vrt_distance_t;
#define VRT_DISTANCE_RP 5
#define VRT_DISTANCE_C(_x) _FXPT_C(vrt_distance_t, _x, VRT_DISTANCE_RP)

static inline vrt_distance_t
double_to_vrt_distance(double num){
  return VRT_DISTANCE_C(num);
}

static inline int32_t
vrt_distance_round_to_int(vrt_distance_t fx){
  return _FXPT_TO_INT(fx, VRT_DISTANCE_C(1));
}

static inline double
vrt_distance_to_double(vrt_distance_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_DISTANCE_C(1));
}

/*
 * The VRT Velocity type (meters/second)
 */
typedef int32_t vrt_velocity_t;
#define VRT_VELOCITY_RP 16
#define VRT_VELOCITY_C(_x) _FXPT_C(vrt_velocity_t, _x, VRT_VELOCITY_RP)

static inline vrt_velocity_t
double_to_vrt_velocity(double num){
  return VRT_VELOCITY_C(num);
}

static inline int32_t
vrt_velocity_round_to_int(vrt_velocity_t fx){
  return _FXPT_TO_INT(fx, VRT_VELOCITY_C(1));
}

static inline double
vrt_velocity_to_double(vrt_velocity_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_VELOCITY_C(1));
}

/*
 * The VRT Angle type (degrees)
 */
typedef int32_t vrt_angle_t;
#define VRT_ANGLE_RP 22
#define VRT_ANGLE_C(_x) _FXPT_C(vrt_angle_t, _x, VRT_ANGLE_RP)

static inline vrt_angle_t
double_to_vrt_angle(double num){
  return VRT_ANGLE_C(num);
}

static inline int16_t
vrt_angle_round_to_int(vrt_angle_t fx){
  return _FXPT_TO_INT(fx, VRT_ANGLE_C(1));
}

static inline double
vrt_angle_to_double(vrt_angle_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_ANGLE_C(1));
}

/*
 * The VRT Hertz type
 */
typedef int64_t vrt_hertz_t;
#define VRT_HERTZ_RP 20
#define VRT_HERTZ_C(_x) _FXPT_C(vrt_hertz_t, _x, VRT_HERTZ_RP)

static inline vrt_hertz_t
double_to_vrt_hertz(double num){
  return VRT_HERTZ_C(num);
}

static inline int64_t
vrt_hertz_round_to_int(vrt_hertz_t fx){
  return _FXPT_TO_INT(fx, VRT_HERTZ_C(1));
}

static inline double
vrt_hertz_to_double(vrt_hertz_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_HERTZ_C(1));
}

/*
 * The VRT dB (& dBm) type
 */
typedef int16_t vrt_db_t;
#define VRT_DB_RP 7
#define VRT_DB_C(_x) _FXPT_C(vrt_db_t, _x, VRT_DB_RP)

static inline vrt_db_t
double_to_vrt_db(double num){
  return VRT_DB_C(num);
}

static inline int16_t
vrt_db_round_to_int(vrt_db_t fx){
  return _FXPT_TO_INT(fx, VRT_DB_C(1));
}

static inline double
vrt_db_to_double(vrt_db_t fx){
  return _FXPT_TO_DOUBLE(fx, VRT_DB_C(1));
}

/*
 * The VRT Temperature type (Celsius)
 */
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

/*
 * The VRT Gain type (7.1.5.10 Gain -- two Q10.6 dB fields)
 */
typedef uint32_t vrt_gain_t;

static inline uint32_t
vrt_make_gain(vrt_db_t stage1, vrt_db_t stage2)
{
  return (((uint32_t) stage2) << 16) | (stage1 & 0xffff);
}

static inline vrt_db_t
vrt_gain_stage1(uint32_t field)
{
  return (vrt_db_t)(field & 0xffff);
}

static inline vrt_db_t
vrt_gain_stage2(uint32_t field)
{
  return (vrt_db_t)((field >> 16) & 0xffff);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VRT_TYPES_H */
