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

#ifndef INCLUDED_FEC_SCL_LIST_H
#define INCLUDED_FEC_SCL_LIST_H

#include <vector>

namespace gr {
  namespace fec {
    namespace code {
      namespace polar {

        struct path {
          path();
          ~path();
          float path_metric;
          bool owns_vectors;
          bool is_active;
          float* llr_vec;
          unsigned char* u_vec;
        };

        /*!
         * \brief List implementation for Successive Cancellation List decoders
         *
         */
        class scl_list{
          const unsigned int d_list_size;
          const unsigned int d_block_size;
          const unsigned int d_block_power;
          const unsigned int d_num_buff_elements;
          std::vector<path*> d_path_list;
          unsigned int d_active_path_counter;
          unsigned int d_active_pos;

          float update_path_metric(const float last_pm, const float llr, const float ui) const;
          void duplicate_path(path* target, const path* original);
          void branch_paths(path* target, path* original, const float llr);
          void steal_vector_ownership(path* target, path* original);
          void reset();

          // comparator for std::sort
          static bool path_compare(path* first, path* second) {
            return first->path_metric < second->path_metric;
          };

        public:
          scl_list(const unsigned int list_size, const unsigned int block_size,
                   const unsigned int block_power);
          virtual
          ~scl_list();
          const unsigned int size() const {return d_list_size;};
          const unsigned int active_size() const {return d_active_path_counter;};

          path* initial_path() const {return d_path_list[0];};
          path* next_active_path(){return d_path_list[d_active_pos++];};
          void set_frozen_bit(const unsigned char frozen_bit, const int bit_pos);
          void set_info_bit(const int bit_pos);
          const path* optimal_path();
        };

      } /* namespace polar */
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_SCL_LIST_H */
