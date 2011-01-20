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

/************************************************************/
/* gr_constellation                                         */
/*                                                          */
/* Decision maker uses nearest-point method.                */
/************************************************************/

class gr_constellation;
typedef boost::shared_ptr<gr_constellation> gr_constellation_sptr;

// public constructor
gr_constellation_sptr 
  gr_make_constellation (std::vector<gr_complex> constellation);

class gr_constellation : public boost::enable_shared_from_this<gr_constellation>
//class gr_constellation
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
    //return gr_constellation_sptr(this);
    return shared_from_this();
  }  

 protected:

  std::vector<gr_complex> d_constellation;
  
 private:
  friend gr_constellation_sptr
  gr_make_constellation (std::vector<gr_complex> constellation);
};

/************************************************************/
/* gr_constellation_sector                                  */
/*                                                          */
/* An abstract class.                                       */
/* Constellation space is divided into sectors.             */
/* Each sector is associated with the nearest constellation */
/* point.                                                   */
/************************************************************/

class gr_constellation_sector : public gr_constellation
{
 public:

  gr_constellation_sector (std::vector<gr_complex> constellation,
			   unsigned int n_sectors);

  unsigned int decision_maker (gr_complex sample);

  // protected:

  virtual unsigned int get_sector (gr_complex sample) = 0;
  
  virtual unsigned int calc_sector_value (unsigned int sector) = 0;

  void find_sector_values ();

  unsigned int n_sectors;

  // private:

  std::vector<unsigned int> sector_values;

};

/************************************************************/
/* gr_constellation_rect                                    */
/*                                                          */
/* Constellation space is divided into rectangular sectors. */
/* Each sector is associated with the nearest constellation */
/* point.                                                   */
/* Works well for square QAM.                               */
/* Works for any generic constellation provided sectors are */
/* not too large.                                           */
/************************************************************/

class gr_constellation_rect;
typedef boost::shared_ptr<gr_constellation_rect> gr_constellation_rect_sptr;

// public constructor
gr_constellation_rect_sptr 
gr_make_constellation_rect (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			    float width_real_sectors, float width_imag_sectors);

class gr_constellation_rect : public gr_constellation_sector
{
 public:

  gr_constellation_rect (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors);

  // protected:

  unsigned int get_sector (gr_complex sample);
  
  unsigned int calc_sector_value (unsigned int sector);

  // private:

  unsigned int n_real_sectors;
  unsigned int n_imag_sectors;
  float d_width_real_sectors;
  float d_width_imag_sectors;

  friend gr_constellation_rect_sptr
  gr_make_constellation_rect (std::vector<gr_complex> constellation, unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors);
  
};

/************************************************************/
/* gr_constellation_psk                                     */
/*                                                          */
/* Constellation space is divided into pie slices sectors.  */
/* Each slice is associated with the nearest constellation  */
/* point.                                                   */
/* Works well for PSK but nothing else.                     */
/* Assumes that there is a constellation point at 1.        */
/************************************************************/

class gr_constellation_psk;
typedef boost::shared_ptr<gr_constellation_psk> gr_constellation_psk_sptr;

// public constructor
gr_constellation_psk_sptr 
gr_make_constellation_psk (std::vector<gr_complex> constellation, unsigned int n_sectors);

class gr_constellation_psk : public gr_constellation_sector
{
 public:

  gr_constellation_psk (std::vector<gr_complex> constellation, unsigned int n_sectors);

  // protected:

  unsigned int get_sector (gr_complex sample);
  
  unsigned int calc_sector_value (unsigned int sector);

  // private:

  friend gr_constellation_psk_sptr
  gr_make_constellation_psk (std::vector<gr_complex> constellation, unsigned int n_sectors);
  
};

#endif
