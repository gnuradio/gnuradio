/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#include "transcendental_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <complex> //complex math
#include <cmath> //real math
#include <map>

namespace gr {
  namespace blocks {

    /***********************************************************************
     * work function creation and registration
     **********************************************************************/
    struct map_val_type
    {
      work_fcn_type work_fcn;
      size_t io_size;
    };
    typedef std::map<std::string, map_val_type> map_type;

    //construct map on first use idiom
    static map_type &
    get_map(void)
    {
      static map_type map;
      return map;
    }

    //static initialization of this object registers a function
    struct transcendental_registrant
    {
      transcendental_registrant(const std::string &key,
                                const work_fcn_type &work_fcn,
                                const size_t io_size)
      {
        map_val_type val;
        val.work_fcn = work_fcn;
        val.io_size = io_size;
        get_map()[key] = val;
      }
    };

    //macro to create a work function and register it
#define REGISTER_FUNCTION(__fcn__, __type__, __key__)                   \
    static int __key__ ## _work(                                        \
        int noutput_items,                                              \
        gr_vector_const_void_star &input_items,                         \
        gr_vector_void_star &output_items)                              \
    {                                                                   \
      const __type__ *in = (const __type__ *) input_items[0];           \
      __type__ *out = (__type__ *) output_items[0];                     \
      for (size_t i = 0; i < size_t(noutput_items); i++){               \
        out[i] = std::__fcn__(in[i]);                                   \
      }                                                                 \
      return noutput_items;                                             \
    }                                                                   \
    transcendental_registrant __key__ ## _registrant(#__key__, &__key__ ## _work, sizeof(__type__));

    //register work functions for real types
#define REGISTER_REAL_FUNCTIONS(__fcn__)                        \
    REGISTER_FUNCTION(__fcn__, float, __fcn__ ## _float)        \
    REGISTER_FUNCTION(__fcn__, double, __fcn__ ## _double)

    //register work functions for complex types
#define REGISTER_COMPLEX_FUNCTIONS(__fcn__)                             \
    REGISTER_FUNCTION(__fcn__, std::complex<float>, __fcn__ ## _complex_float) \
    REGISTER_FUNCTION(__fcn__, std::complex<double>, __fcn__ ## _complex_double)

    //register both complex and real
#define REGISTER_FUNCTIONS(__fcn__)  \
    REGISTER_REAL_FUNCTIONS(__fcn__)            \
    REGISTER_COMPLEX_FUNCTIONS(__fcn__)

    //create and register transcendental work functions
    REGISTER_FUNCTIONS(cos)
    REGISTER_FUNCTIONS(sin)
    REGISTER_FUNCTIONS(tan)
    REGISTER_REAL_FUNCTIONS(acos)
    REGISTER_REAL_FUNCTIONS(asin)
    REGISTER_REAL_FUNCTIONS(atan)
    REGISTER_FUNCTIONS(cosh)
    REGISTER_FUNCTIONS(sinh)
    REGISTER_FUNCTIONS(tanh)
    REGISTER_FUNCTIONS(exp)
    REGISTER_FUNCTIONS(log)
    REGISTER_FUNCTIONS(log10)
    REGISTER_FUNCTIONS(sqrt)


    transcendental::sptr
    transcendental::make(const std::string &name,
                         const std::string &type)
    {
      //search for an entry in the map
      const std::string key = name + "_" + type;
      const bool has_key = get_map().count(key) != 0;
      if(!has_key)
        throw std::runtime_error("could not find transcendental function for " + key);

      //make a new block with found work function
      return gnuradio::get_initial_sptr
        (new transcendental_impl(get_map()[key].work_fcn, get_map()[key].io_size));
    }

    transcendental_impl::transcendental_impl(const work_fcn_type &work_fcn,
                                             const size_t io_size)
      : sync_block("transcendental",
                      io_signature::make(1, 1, io_size),
                      io_signature::make(1, 1, io_size)),
        _work_fcn(work_fcn)
    {
      // NOP
    }

    transcendental_impl::~transcendental_impl()
    {
    }

    int
    transcendental_impl::work(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      return _work_fcn(noutput_items, input_items, output_items);
    }

  } /* namespace blocks */
} /* namespace gr */
