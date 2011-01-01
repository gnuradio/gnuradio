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

#ifndef INCLUDED_GR_CONSTELLATION_H
#define	INCLUDED_GR_CONSTELLATION_H

#include <vector>
#include <math.h>
#include <gr_complex.h>
#include <boost/enable_shared_from_this.hpp>

class gr_constellation;
typedef boost::shared_ptr<gr_constellation> gr_constellation_sptr;

// public constructor
gr_constellation_sptr 
  gr_make_constellation (std::vector<gr_complex> constellation);

class gr_constellation : public boost::enable_shared_from_this<gr_constellation>
{
 public:

  gr_constellation (std::vector<gr_complex> constellation);
  
  //! Returns the set of points in this constellation.
  std::vector<gr_complex> points() { return d_constellation;}
  
  //! Returns the constellation point that matches best.
  //! Also calculates the phase error.
  virtual unsigned int decision_maker (gr_complex sample);
  
  unsigned int bits_per_symbol () {
    return floor(log(d_constellation.size())/log(2));
  }

  gr_constellation_sptr base() {
    return shared_from_this();
  }  

 protected:

  std::vector<gr_complex> d_constellation;
  
 private:
  friend gr_constellation_sptr
  gr_make_constellation (std::vector<gr_complex> constellation);
};

class gr_constellation_sector;
typedef boost::shared_ptr<gr_constellation_sector> gr_constellation_sector_sptr;

// public constructor
gr_constellation_sector_sptr 
gr_make_constellation_sector (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			      float width_real_sectors, float width_imag_sectors);

class gr_constellation_sector : public gr_constellation
{
 public:

  gr_constellation_sector (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors);

  unsigned int decision_maker (gr_complex sample);

 protected:

  virtual unsigned int get_sector (gr_complex sample);
  
  virtual unsigned int calc_sector_value (unsigned int sector);

  void find_sector_values ();

 private:

  std::vector<unsigned int> sector_values;
  unsigned int n_sectors;
  unsigned int n_real_sectors;
  unsigned int n_imag_sectors;
  float d_width_real_sectors;
  float d_width_imag_sectors;

  friend gr_constellation_sector_sptr
  gr_make_constellation_sector (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors);
  
};

#endif
