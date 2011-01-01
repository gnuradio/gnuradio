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

%template(gr_complex_vector) std::vector<gr_complex>;
%template(unsigned_int_vector) std::vector<unsigned int>;

class gr_constellation;
typedef boost::shared_ptr<gr_constellation> gr_constellation_sptr;
%template(gr_constellation_sptr) boost::shared_ptr<gr_constellation>;
%rename(constellation) gr_make_constellation;
gr_constellation_sptr gr_make_constellation(std::vector<gr_complex> constellation);
%ignore gr_constellation;

class gr_constellation
{
public:
  gr_constellation (std::vector<gr_complex> constellation);
  std::vector<gr_complex> points();
  unsigned int decision_maker (gr_complex sample);  
  unsigned int bits_per_symbol ();
  gr_constellation_sptr base ();
};

class gr_constellation_sector;
typedef boost::shared_ptr<gr_constellation_sector> gr_constellation_sector_sptr;
%template(gr_constellation_sector_sptr) boost::shared_ptr<gr_constellation_sector>;
%rename(constellation_sector) gr_make_constellation_sector;
gr_constellation_sector_sptr gr_make_constellation_sector(std::vector<gr_complex> constellation_sector,
							  unsigned int real_sectors, unsigned int imag_sectors,
							  float width_real_sectors, float width_imag_sectors);
%ignore gr_constellation_sector;

class gr_constellation_sector : public gr_constellation
{
public:
  gr_constellation_sector (std::vector<gr_complex> constellation,
			   unsigned int real_sectors, unsigned int imag_sectors,
			   float width_real_sectors, float width_imag_sectors);
  std::vector<gr_complex> points ();
  unsigned int decision_maker (gr_complex sample);  
  unsigned int bits_per_symbol ();
  gr_constellation_sptr base ();
};
