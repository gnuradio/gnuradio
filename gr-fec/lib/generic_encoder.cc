/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#include <gnuradio/fec/generic_encoder.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    generic_encoder::~generic_encoder()
    {
    }

    int
    get_encoder_output_size(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_output_size();
    }

    int
    get_encoder_input_size(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_input_size();
    }

  } /* namespace fec */
} /* namespace gr */
