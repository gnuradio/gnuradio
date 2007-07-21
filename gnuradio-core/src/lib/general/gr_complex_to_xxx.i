/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,complex_to_float);
gr_complex_to_float_sptr gr_make_complex_to_float (unsigned int vlen=1);
class gr_complex_to_float : public gr_sync_block
{
  gr_complex_to_float (unsigned int vlen);
};

GR_SWIG_BLOCK_MAGIC(gr,complex_to_real);
gr_complex_to_real_sptr gr_make_complex_to_real (unsigned int vlen=1);
class gr_complex_to_real : public gr_sync_block
{
  gr_complex_to_real (unsigned int vlen);
};

GR_SWIG_BLOCK_MAGIC(gr,complex_to_imag);
gr_complex_to_imag_sptr gr_make_complex_to_imag (unsigned int vlen=1);
class gr_complex_to_imag : public gr_sync_block
{
  gr_complex_to_imag (unsigned int vlen);
};

GR_SWIG_BLOCK_MAGIC(gr,complex_to_mag);
gr_complex_to_mag_sptr gr_make_complex_to_mag (unsigned int vlen=1);
class gr_complex_to_mag : public gr_sync_block
{
  gr_complex_to_mag (unsigned int vlen);
};

GR_SWIG_BLOCK_MAGIC(gr,complex_to_mag_squared);
gr_complex_to_mag_squared_sptr gr_make_complex_to_mag_squared (unsigned int vlen=1);
class gr_complex_to_mag_squared : public gr_sync_block
{
  gr_complex_to_mag_squared (unsigned int vlen);
};

GR_SWIG_BLOCK_MAGIC(gr,complex_to_arg);
gr_complex_to_arg_sptr gr_make_complex_to_arg (unsigned int vlen=1);
class gr_complex_to_arg : public gr_sync_block
{
  gr_complex_to_arg (unsigned int vlen);
};

