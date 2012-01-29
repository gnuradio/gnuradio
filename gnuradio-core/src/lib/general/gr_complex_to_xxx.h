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

#ifndef INCLUDED_GR_COMPLEX_TO_XXX_H
#define INCLUDED_GR_COMPLEX_TO_XXX_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class gr_complex_to_float;
class gr_complex_to_real;
class gr_complex_to_imag;
class gr_complex_to_mag;
class gr_complex_to_mag_squared;
class gr_complex_to_arg;

typedef boost::shared_ptr<gr_complex_to_float> gr_complex_to_float_sptr;
typedef boost::shared_ptr<gr_complex_to_real> gr_complex_to_real_sptr;
typedef boost::shared_ptr<gr_complex_to_imag> gr_complex_to_imag_sptr;
typedef boost::shared_ptr<gr_complex_to_mag> gr_complex_to_mag_sptr;
typedef boost::shared_ptr<gr_complex_to_mag_squared> gr_complex_to_mag_squared_sptr;
typedef boost::shared_ptr<gr_complex_to_arg> gr_complex_to_arg_sptr;

GR_CORE_API gr_complex_to_float_sptr gr_make_complex_to_float (unsigned int vlen=1);
GR_CORE_API gr_complex_to_real_sptr gr_make_complex_to_real (unsigned int vlen=1);
GR_CORE_API gr_complex_to_imag_sptr gr_make_complex_to_imag (unsigned int vlen=1);
GR_CORE_API gr_complex_to_mag_sptr gr_make_complex_to_mag (unsigned int vlen=1);
GR_CORE_API gr_complex_to_mag_squared_sptr gr_make_complex_to_mag_squared (unsigned int vlen=1);
GR_CORE_API gr_complex_to_arg_sptr gr_make_complex_to_arg (unsigned int vlen=1);

/*!
 * \brief convert a stream of gr_complex to 1 or 2 streams of float
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_float : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_float_sptr gr_make_complex_to_float (unsigned int vlen);
  gr_complex_to_float (unsigned int vlen);

  unsigned int	d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

/*!
 * \brief complex in, real out (float)
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_real : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_real_sptr gr_make_complex_to_real (unsigned int vlen);
  gr_complex_to_real (unsigned int vlen);

  unsigned int	d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

/*!
 * \brief complex in, imaginary out (float)
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_imag : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_imag_sptr gr_make_complex_to_imag (unsigned int vlen);
  gr_complex_to_imag (unsigned int vlen);

  unsigned int	d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

/*!
 * \brief complex in, magnitude out (float)
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_mag : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_mag_sptr
    gr_make_complex_to_mag (unsigned int vlen);
  gr_complex_to_mag (unsigned int vlen);

  unsigned int d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

/*!
 * \brief complex in, magnitude squared out (float)
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_mag_squared : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_mag_squared_sptr gr_make_complex_to_mag_squared (unsigned int vlen);
  gr_complex_to_mag_squared (unsigned int vlen);

  unsigned int	d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

/*!
 * \brief complex in, angle out (float) 
 * \ingroup converter_blk
 * \param vlen	vector len (default 1)
 */
class GR_CORE_API gr_complex_to_arg : public gr_sync_block
{
  friend GR_CORE_API gr_complex_to_arg_sptr gr_make_complex_to_arg (unsigned int vlen);
  gr_complex_to_arg (unsigned int vlen);

  unsigned int	d_vlen;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_COMPLEX_TO_XXX_H */
