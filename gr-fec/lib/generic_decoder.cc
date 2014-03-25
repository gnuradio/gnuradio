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

#include <gnuradio/fec/generic_decoder.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    generic_decoder::generic_decoder(std::string name)
    {
      d_name = name;
      my_id = base_unique_id++;
    }

    generic_decoder::~generic_decoder()
    {
    }

    int
    generic_decoder::get_history()
    {
      return 0;
    }

    float
    generic_decoder::get_shift()
    {
      return 0.0;
    }

    const char*
    generic_decoder::get_conversion()
    {
      return "none";
    }

    int
    generic_decoder::get_input_item_size()
    {
      return 4;
    }

    int
    generic_decoder::get_output_item_size()
    {
      return 1;
    }

    const char*
    generic_decoder::get_output_conversion()
    {
      return "none";
    }

    int generic_decoder::base_unique_id = 1;
    int
    generic_decoder::unique_id()
    {
      return my_id;
    }

    /*******************************************************
     * Static functions
     ******************************************************/
    int
    get_decoder_output_size(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_output_size();
    }

    int
    get_history(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_history();
    }

    int
    get_decoder_input_size(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_input_size();
    }

    int
    get_decoder_output_item_size(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_output_item_size();
    }

    int
    get_decoder_input_item_size(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_input_item_size();
    }

    float
    get_shift(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_shift();
    }

    const char*
    get_conversion(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_conversion();
    }

    const char*
    get_output_conversion(generic_decoder::sptr my_decoder)
    {
      return my_decoder->get_output_conversion();
    }

  } /* namespace fec */
} /* namespace gr */
