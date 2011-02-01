/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include <gr_constellation.h>
#include <gr_math.h>
#include <gr_complex.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

#define M_TWOPI (2*M_PI)
#define SQRT_TWO 0.707107

gr_constellation_sptr 
gr_make_constellation(std::vector<gr_complex> constellation)
{
  return gr_constellation_sptr(new gr_constellation (constellation));
}

// Base Constellation Class

gr_constellation::gr_constellation (std::vector<gr_complex> constellation) :
  d_constellation(constellation)
{
}

gr_constellation::gr_constellation ()
{
}

unsigned int get_closest_point(std::vector<gr_complex> constellation, gr_complex sample) {

  unsigned int table_size = constellation.size();
  unsigned int min_index = 0;
  float min_euclid_dist;
  float euclid_dist;
    
  min_euclid_dist = norm(sample - constellation[0]); 
  min_index = 0; 
  for (unsigned int j = 1; j < table_size; j++){
    euclid_dist = norm(sample - constellation[j]);
    if (euclid_dist < min_euclid_dist){
      min_euclid_dist = euclid_dist;
      min_index = j;
    }
  }
  return min_index;
}

// Chooses points base on shortest distance.
// Inefficient.
unsigned int gr_constellation::decision_maker(gr_complex sample)
{
  unsigned int min_index;
  min_index = get_closest_point(d_constellation, sample);
  return min_index;
}

gr_constellation_sector::gr_constellation_sector (std::vector<gr_complex> constellation,
						  unsigned int n_sectors) :
  gr_constellation(constellation),
  n_sectors(n_sectors)
{
}

unsigned int gr_constellation_sector::decision_maker (gr_complex sample) {
  unsigned int sector;
  sector = get_sector(sample);
  return sector_values[sector];
}

void gr_constellation_sector::find_sector_values () {
  unsigned int i;
  sector_values.clear();
  for (i=0; i<n_sectors; i++) {
    sector_values.push_back(calc_sector_value(i));
  }
}

gr_constellation_rect_sptr 
gr_make_constellation_rect(std::vector<gr_complex> constellation,
			   unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors)
{
  return gr_constellation_rect_sptr(new gr_constellation_rect (constellation, real_sectors, imag_sectors, width_real_sectors, width_imag_sectors));
  }

gr_constellation_rect::gr_constellation_rect (std::vector<gr_complex> constellation,
					      unsigned int real_sectors, unsigned int imag_sectors,
					      float width_real_sectors, float width_imag_sectors) :
  gr_constellation_sector(constellation, real_sectors * imag_sectors),
  n_real_sectors(real_sectors), n_imag_sectors(imag_sectors),
  d_width_real_sectors(width_real_sectors), d_width_imag_sectors(width_imag_sectors)
{
  find_sector_values();
}

unsigned int gr_constellation_rect::get_sector (gr_complex sample) {
  int real_sector, imag_sector;
  unsigned int sector;
  real_sector = int(real(sample)/d_width_real_sectors + n_real_sectors/2.0);
  if (real_sector < 0) real_sector = 0;
  if (real_sector >= n_real_sectors) real_sector = n_real_sectors-1;
  imag_sector = int(imag(sample)/d_width_imag_sectors + n_imag_sectors/2.0);
  if (imag_sector < 0) imag_sector = 0;
  if (imag_sector >= n_imag_sectors) imag_sector = n_imag_sectors-1;
  sector = real_sector * n_imag_sectors + imag_sector;
  return sector;
}
  
unsigned int gr_constellation_rect::calc_sector_value (unsigned int sector) {
  unsigned int real_sector, imag_sector;
  gr_complex sector_center;
  unsigned int closest_point;
  real_sector = float(sector)/n_imag_sectors;
  imag_sector = sector - real_sector * n_imag_sectors;
  sector_center = gr_complex((real_sector + 0.5 - n_real_sectors/2.0) * d_width_real_sectors,
			     (imag_sector + 0.5 - n_imag_sectors/2.0) * d_width_imag_sectors);
  closest_point = get_closest_point(d_constellation, sector_center);
  return closest_point;
}


gr_constellation_psk_sptr 
gr_make_constellation_psk(std::vector<gr_complex> constellation, unsigned int n_sectors)
{
  return gr_constellation_psk_sptr(new gr_constellation_psk (constellation, n_sectors));
}

gr_constellation_psk::gr_constellation_psk (std::vector<gr_complex> constellation,
					    unsigned int n_sectors) :
  gr_constellation_sector(constellation, n_sectors)
{
  find_sector_values();
}

unsigned int gr_constellation_psk::get_sector (gr_complex sample) {
  float phase = arg(sample);
  float width = M_TWOPI / n_sectors;
  int sector = floor(phase/width + 0.5);
  unsigned int u_sector;
  if (sector < 0) sector += n_sectors;
  u_sector = sector;
  //  std::cout << phase << " " << width << " " << sector << std::endl;
  return sector;
}
  
unsigned int gr_constellation_psk::calc_sector_value (unsigned int sector) {
  float phase = sector * M_TWOPI / n_sectors;
  gr_complex sector_center = gr_complex(cos(phase), sin(phase));
  unsigned int closest_point = get_closest_point(d_constellation, sector_center);
  //  std::cout << phase << " " << sector_center << " " << closest_point << std::endl;
  return closest_point;
}


gr_constellation_bpsk_sptr 
gr_make_constellation_bpsk()
{
  return gr_constellation_bpsk_sptr(new gr_constellation_bpsk ());
}

gr_constellation_bpsk::gr_constellation_bpsk ()
{
  d_constellation.resize(2);
  d_constellation[0] = gr_complex(-1, 0);
  d_constellation[1] = gr_complex(1, 0);
}

unsigned int gr_constellation_bpsk::decision_maker(gr_complex sample)
{
  return (real(sample) > 0);
}


gr_constellation_qpsk_sptr 
gr_make_constellation_qpsk()
{
  return gr_constellation_qpsk_sptr(new gr_constellation_qpsk ());
}

gr_constellation_qpsk::gr_constellation_qpsk ()
{
  d_constellation.resize(4);
  // Gray-coded
  d_constellation[0] = gr_complex(-SQRT_TWO, -SQRT_TWO);
  d_constellation[1] = gr_complex(SQRT_TWO, -SQRT_TWO);
  d_constellation[2] = gr_complex(-SQRT_TWO, SQRT_TWO);
  d_constellation[3] = gr_complex(SQRT_TWO, SQRT_TWO);
}

unsigned int gr_constellation_qpsk::decision_maker(gr_complex sample)
{
  // Real component determines small bit.
  // Imag component determines big bit.
  return 2*(imag(sample)>0) + (real(sample)>0);
}
