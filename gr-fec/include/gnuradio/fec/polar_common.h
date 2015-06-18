/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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


#ifndef INCLUDED_FEC_POLAR_COMMON_H
#define INCLUDED_FEC_POLAR_COMMON_H

#include <gnuradio/fec/api.h>

// Forward declaration for those objects. SWIG doesn't like them to be #include'd.
namespace gr {
  namespace blocks {
    namespace kernel {
      class pack_k_bits;
      class unpack_k_bits;
    }
  }
}

namespace gr {
  namespace fec {

    /*!
     * \brief POLAR code common operations and attributes
     * Erdal Arikan "Channel Polarization: A Method for Contructing Capacity-Achieving Codes for Symmetric Binary-Input Memoryless Channels", 2009
     */
    class FEC_API polar_common
    {
    public:
      polar_common(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions, std::vector<char> frozen_bit_values, bool is_packed = false);
      ~polar_common();

    protected:
      const int block_size()const {return d_block_size;};
      const int block_power()const {return d_block_power;};
      const int num_info_bits() const {return d_num_info_bits;};
      const bool is_packed() const {return d_is_packed;};

      // helper functions
      long bit_reverse(long value, int active_bits) const;
      void print_packed_bit_array(const unsigned char* printed_array, const int num_bytes) const;
      void print_unpacked_bit_array(const unsigned char* bits, const unsigned int num_bytes) const;
      const gr::blocks::kernel::unpack_k_bits* unpacker() const {return d_unpacker;};
      std::vector<int> info_bit_position_vector();

    private:
      int d_block_size; // depending on paper called 'N' or 'm'
      int d_block_power;
      int d_num_info_bits; // mostly abbreviated by 'K'
      bool d_is_packed;
      std::vector<int> d_frozen_bit_positions;
      std::vector<int> d_info_bit_positions;
      std::vector<char> d_frozen_bit_values;
      void initialize_info_bit_position_vector();

      gr::blocks::kernel::pack_k_bits *d_packer;
      gr::blocks::kernel::unpack_k_bits *d_unpacker;
    };

  } // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_COMMON_H */

