/* -*- c++ -*- */
/*
 * Copyright 2010, 2011 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <digital_constellation.h>
#include <digital_metric_type.h>
#include <gr_math.h>
#include <gr_complex.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <float.h>
#include <stdexcept>

#define M_TWOPI (2*M_PI)
#define SQRT_TWO 0.707107

// Base Constellation Class

digital_constellation::digital_constellation (std::vector<gr_complex> constellation,
					      std::vector<unsigned int> pre_diff_code,
					      unsigned int rotational_symmetry,
					      unsigned int dimensionality) :
  d_constellation(constellation),
  d_pre_diff_code(pre_diff_code),
  d_rotational_symmetry(rotational_symmetry),
  d_dimensionality(dimensionality)
{
  if (pre_diff_code.size() == 0)
    d_apply_pre_diff_code = false;
  else if (pre_diff_code.size() != constellation.size())
    throw std::runtime_error ("The constellation and pre-diff code must be of the same length.");
  else
    d_apply_pre_diff_code = true;
  calc_arity();
}

digital_constellation::digital_constellation () :
  d_apply_pre_diff_code(false),
  d_rotational_symmetry(0),
  d_dimensionality(1)
{
  calc_arity();
}

//! Returns the constellation points for a symbol value
void
digital_constellation::map_to_points(unsigned int value, gr_complex *points)
{
  for (unsigned int i=0; i<d_dimensionality; i++)
    points[i] = d_constellation[value*d_dimensionality + i];
}

std::vector<gr_complex>
digital_constellation::map_to_points_v(unsigned int value)
{
  std::vector<gr_complex> points_v;
  points_v.resize(d_dimensionality);
  map_to_points(value, &(points_v[0]));
  return points_v;
}

float
digital_constellation::get_distance(unsigned int index, const gr_complex *sample)
{
  float dist = 0;
  for (unsigned int i=0; i<d_dimensionality; i++) {
    dist += norm(sample[i] - d_constellation[index*d_dimensionality + i]);
  }
  return dist;
}

unsigned int
digital_constellation::get_closest_point(const gr_complex *sample)
{
  unsigned int min_index = 0;
  float min_euclid_dist;
  float euclid_dist;
    
  min_euclid_dist = get_distance(0, sample);
  min_index = 0;
  for (unsigned int j = 1; j < d_arity; j++){
    euclid_dist = get_distance(j, sample);
    if (euclid_dist < min_euclid_dist){
      min_euclid_dist = euclid_dist;
      min_index = j;
    }
  }
  return min_index;
}

unsigned int
digital_constellation::decision_maker_pe(const gr_complex *sample, float *phase_error)
{
  unsigned int index = decision_maker(sample);
  *phase_error = 0;
  for (unsigned int d=0; d<d_dimensionality; d++)
    *phase_error += -arg(sample[d]*conj(d_constellation[index+d]));
  return index;
}

/*
unsigned int digital_constellation::decision_maker_e(const gr_complex *sample, float *error)
{
  unsigned int index = decision_maker(sample);
  *error = 0;
  for (unsigned int d=0; d<d_dimensionality; d++)
    *error += sample[d]*conj(d_constellation[index+d]);
  return index;
}
*/

std::vector<gr_complex> digital_constellation::s_points () {
  if (d_dimensionality != 1)
    throw std::runtime_error ("s_points only works for dimensionality 1 constellations.");
  else
    return d_constellation;
}

std::vector<std::vector<gr_complex> >
digital_constellation::v_points ()
{
  std::vector<std::vector<gr_complex> > vv_const;
  vv_const.resize(d_arity);
  for (unsigned int p=0; p<d_arity; p++) {
    std::vector<gr_complex> v_const;
    v_const.resize(d_dimensionality);
    for (unsigned int d=0; d<d_dimensionality; d++) {
      v_const[d] = d_constellation[p*d_dimensionality+d];
    }
    vv_const[p] = v_const;
  }
  return vv_const;
}

void
digital_constellation::calc_metric(const gr_complex *sample, float *metric,
				   trellis_metric_type_t type)
{
  switch (type){
  case TRELLIS_EUCLIDEAN:
    calc_euclidean_metric(sample, metric);
    break;
  case TRELLIS_HARD_SYMBOL:
    calc_hard_symbol_metric(sample, metric);
    break;
  case TRELLIS_HARD_BIT:
    throw std::runtime_error ("Invalid metric type (not yet implemented).");
    break;
  default:
    throw std::runtime_error ("Invalid metric type.");
  }
}

void
digital_constellation::calc_euclidean_metric(const gr_complex *sample, float *metric)
{
  for (unsigned int o=0; o<d_arity; o++) {
    metric[o] = get_distance(o, sample);
  }
}

void
digital_constellation::calc_hard_symbol_metric(const gr_complex *sample, float *metric)
{
  float minm = FLT_MAX;
  unsigned int minmi = 0;
  for (unsigned int o=0; o<d_arity; o++) {
    float dist = get_distance(o, sample);
    if (dist < minm) {
      minm = dist;
      minmi = o;
    }
  }
  for(unsigned int o=0; o<d_arity; o++) {
    metric[o] = (o==minmi?0.0:1.0);
  }
}

void
digital_constellation::calc_arity ()
{
  if (d_constellation.size() % d_dimensionality != 0)
    throw std::runtime_error ("Constellation vector size must be a multiple of the dimensionality.");    
  d_arity = d_constellation.size()/d_dimensionality;
}

unsigned int
digital_constellation::decision_maker_v (std::vector<gr_complex> sample)
{
  assert(sample.size() == d_dimensionality);
  return decision_maker (&(sample[0]));
}

digital_constellation_calcdist_sptr 
digital_make_constellation_calcdist(std::vector<gr_complex> constellation,
				    std::vector<unsigned int> pre_diff_code,
				    unsigned int rotational_symmetry,
				    unsigned int dimensionality)
{
  return digital_constellation_calcdist_sptr(new digital_constellation_calcdist
					     (constellation, pre_diff_code,
					      rotational_symmetry, dimensionality));
}

digital_constellation_calcdist::digital_constellation_calcdist(std::vector<gr_complex> constellation,
							       std::vector<unsigned int> pre_diff_code,
							       unsigned int rotational_symmetry,
							       unsigned int dimensionality) :
  digital_constellation(constellation, pre_diff_code, rotational_symmetry, dimensionality)
{}

// Chooses points base on shortest distance.
// Inefficient.
unsigned int
digital_constellation_calcdist::decision_maker(const gr_complex *sample)
{
  return get_closest_point(sample);
}

digital_constellation_sector::digital_constellation_sector (std::vector<gr_complex> constellation,
							    std::vector<unsigned int> pre_diff_code,
							    unsigned int rotational_symmetry,
							    unsigned int dimensionality,
							    unsigned int n_sectors) :
  digital_constellation(constellation, pre_diff_code, rotational_symmetry, dimensionality),
  n_sectors(n_sectors)
{
}

unsigned int
digital_constellation_sector::decision_maker (const gr_complex *sample)
{
  unsigned int sector;
  sector = get_sector(sample);
  return sector_values[sector];
}

void
digital_constellation_sector::find_sector_values ()
{
  unsigned int i;
  sector_values.clear();
  for (i=0; i<n_sectors; i++) {
    sector_values.push_back(calc_sector_value(i));
  }
}

digital_constellation_rect_sptr 
digital_make_constellation_rect(std::vector<gr_complex> constellation,
				std::vector<unsigned int> pre_diff_code,
				unsigned int rotational_symmetry,
				unsigned int real_sectors, unsigned int imag_sectors,
				float width_real_sectors, float width_imag_sectors)
{
  return digital_constellation_rect_sptr(new digital_constellation_rect
					 (constellation, pre_diff_code,
					  rotational_symmetry,
					  real_sectors, imag_sectors,
					  width_real_sectors,
					  width_imag_sectors));
  }

digital_constellation_rect::digital_constellation_rect (std::vector<gr_complex> constellation,
							std::vector<unsigned int> pre_diff_code,
							unsigned int rotational_symmetry,
							unsigned int real_sectors, unsigned int imag_sectors,
							float width_real_sectors, float width_imag_sectors) :
  digital_constellation_sector(constellation, pre_diff_code, rotational_symmetry, 1, real_sectors * imag_sectors),
  n_real_sectors(real_sectors), n_imag_sectors(imag_sectors),
  d_width_real_sectors(width_real_sectors), d_width_imag_sectors(width_imag_sectors)
{
  find_sector_values();
}

unsigned int
digital_constellation_rect::get_sector (const gr_complex *sample)
{
  int real_sector, imag_sector;
  unsigned int sector;

  real_sector = int(real(*sample)/d_width_real_sectors + n_real_sectors/2.0);
  if(real_sector < 0)
    real_sector = 0;
  if(real_sector >= (int)n_real_sectors)
    real_sector = n_real_sectors-1;

  imag_sector = int(imag(*sample)/d_width_imag_sectors + n_imag_sectors/2.0);
  if(imag_sector < 0)
    imag_sector = 0;
  if(imag_sector >= (int)n_imag_sectors)
    imag_sector = n_imag_sectors-1;

  sector = real_sector * n_imag_sectors + imag_sector;
  return sector;
}
  
unsigned int
digital_constellation_rect::calc_sector_value (unsigned int sector)
{
  unsigned int real_sector, imag_sector;
  gr_complex sector_center;
  unsigned int closest_point;
  real_sector = float(sector)/n_imag_sectors;
  imag_sector = sector - real_sector * n_imag_sectors;
  sector_center = gr_complex((real_sector + 0.5 - n_real_sectors/2.0) * d_width_real_sectors,
			     (imag_sector + 0.5 - n_imag_sectors/2.0) * d_width_imag_sectors);
  closest_point = get_closest_point(&sector_center);
  return closest_point;
}


digital_constellation_psk_sptr 
digital_make_constellation_psk(std::vector<gr_complex> constellation, 
			       std::vector<unsigned int> pre_diff_code,
			       unsigned int n_sectors)
{
  return digital_constellation_psk_sptr(new digital_constellation_psk
					(constellation, pre_diff_code,
					 n_sectors));
}

digital_constellation_psk::digital_constellation_psk (std::vector<gr_complex> constellation,
						      std::vector<unsigned int> pre_diff_code,
						      unsigned int n_sectors) :
  digital_constellation_sector(constellation, pre_diff_code, constellation.size(), 1, n_sectors)
{
  find_sector_values();
}

unsigned int
digital_constellation_psk::get_sector (const gr_complex *sample)
{
  float phase = arg(*sample);
  float width = M_TWOPI / n_sectors;
  int sector = floor(phase/width + 0.5);
  if (sector < 0)
    sector += n_sectors;
  return sector;
}
  
unsigned int
digital_constellation_psk::calc_sector_value (unsigned int sector)
{
  float phase = sector * M_TWOPI / n_sectors;
  gr_complex sector_center = gr_complex(cos(phase), sin(phase));
  unsigned int closest_point = get_closest_point(&sector_center);
  return closest_point;
}


digital_constellation_bpsk_sptr 
digital_make_constellation_bpsk()
{
  return digital_constellation_bpsk_sptr(new digital_constellation_bpsk ());
}

digital_constellation_bpsk::digital_constellation_bpsk ()
{
  d_constellation.resize(2);
  d_constellation[0] = gr_complex(-1, 0);
  d_constellation[1] = gr_complex(1, 0);
  d_rotational_symmetry = 2;
  d_dimensionality = 1;
  calc_arity();
}

unsigned int
digital_constellation_bpsk::decision_maker(const gr_complex *sample)
{
  return (real(*sample) > 0);
}


digital_constellation_qpsk_sptr 
digital_make_constellation_qpsk()
{
  return digital_constellation_qpsk_sptr(new digital_constellation_qpsk ());
}

digital_constellation_qpsk::digital_constellation_qpsk ()
{
  d_constellation.resize(4);
  // Gray-coded
  d_constellation[0] = gr_complex(-SQRT_TWO, -SQRT_TWO);
  d_constellation[1] = gr_complex(SQRT_TWO, -SQRT_TWO);
  d_constellation[2] = gr_complex(-SQRT_TWO, SQRT_TWO);
  d_constellation[3] = gr_complex(SQRT_TWO, SQRT_TWO);
  
  /*
  d_constellation[0] = gr_complex(SQRT_TWO, SQRT_TWO);
  d_constellation[1] = gr_complex(-SQRT_TWO, SQRT_TWO);
  d_constellation[2] = gr_complex(SQRT_TWO, -SQRT_TWO);
  d_constellation[3] = gr_complex(SQRT_TWO, -SQRT_TWO);
  */

  d_pre_diff_code.resize(4);
  d_pre_diff_code[0] = 0x0;
  d_pre_diff_code[1] = 0x2;
  d_pre_diff_code[2] = 0x3;
  d_pre_diff_code[3] = 0x1;

  d_rotational_symmetry = 4;
  d_dimensionality = 1;
  calc_arity();
}

unsigned int
digital_constellation_qpsk::decision_maker(const gr_complex *sample)
{
  // Real component determines small bit.
  // Imag component determines big bit.
  return 2*(imag(*sample)>0) + (real(*sample)>0);

  /*
  bool a = real(*sample) > 0;
  bool b = imag(*sample) > 0;
  if(a) {
    if(b)
      return 0x0;
    else
      return 0x1;
  }
  else {
    if(b)
      return 0x2;
    else
      return 0x3;
  }
  */
}


/********************************************************************/


digital_constellation_dqpsk_sptr 
digital_make_constellation_dqpsk()
{
  return digital_constellation_dqpsk_sptr(new digital_constellation_dqpsk ());
}

digital_constellation_dqpsk::digital_constellation_dqpsk ()
{
  // This constellation is not gray coded, which allows
  // us to use differential encodings (through gr_diff_encode and
  // gr_diff_decode) on the symbols.
  d_constellation.resize(4);
  d_constellation[0] = gr_complex(+SQRT_TWO, +SQRT_TWO);
  d_constellation[1] = gr_complex(-SQRT_TWO, +SQRT_TWO);
  d_constellation[2] = gr_complex(-SQRT_TWO, -SQRT_TWO);
  d_constellation[3] = gr_complex(+SQRT_TWO, -SQRT_TWO);

  // Use this mapping to convert to gray code before diff enc.
  d_pre_diff_code.resize(4);
  d_pre_diff_code[0] = 0x0;
  d_pre_diff_code[1] = 0x1;
  d_pre_diff_code[2] = 0x3;
  d_pre_diff_code[3] = 0x2;
  d_apply_pre_diff_code = true;

  d_rotational_symmetry = 4;
  d_dimensionality = 1;
  calc_arity();
}

unsigned int
digital_constellation_dqpsk::decision_maker(const gr_complex *sample)
{
  // Slower deicison maker as we can't slice along one axis.
  // Maybe there's a better way to do this, still.

  bool a = real(*sample) > 0;
  bool b = imag(*sample) > 0;
  if(a) {
    if(b)
      return 0x0;
    else
      return 0x3;
  }
  else {
    if(b)
      return 0x1;
    else
      return 0x2;
  }
}

digital_constellation_8psk_sptr 
digital_make_constellation_8psk()
{
  return digital_constellation_8psk_sptr(new digital_constellation_8psk ());
}

digital_constellation_8psk::digital_constellation_8psk ()
{
  float angle = M_PI/8.0;
  d_constellation.resize(8);
  // Gray-coded
  d_constellation[0] = gr_complex(cos( 1*angle), sin( 1*angle));
  d_constellation[1] = gr_complex(cos( 7*angle), sin( 7*angle));
  d_constellation[2] = gr_complex(cos(15*angle), sin(15*angle));
  d_constellation[3] = gr_complex(cos( 9*angle), sin( 9*angle));
  d_constellation[4] = gr_complex(cos( 3*angle), sin( 3*angle));
  d_constellation[5] = gr_complex(cos( 5*angle), sin( 5*angle));
  d_constellation[6] = gr_complex(cos(13*angle), sin(13*angle));
  d_constellation[7] = gr_complex(cos(11*angle), sin(11*angle));
  d_rotational_symmetry = 8;
  d_dimensionality = 1;
  calc_arity();
}

unsigned int
digital_constellation_8psk::decision_maker(const gr_complex *sample)
{
  unsigned int ret = 0;

  float re = sample->real();
  float im = sample->imag();

  if(fabsf(re) <= fabsf(im))
    ret  = 4;
  if(re <= 0)
    ret |= 1;
  if(im <= 0)
    ret |= 2;

  return ret;
}
