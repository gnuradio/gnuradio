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

#include <scl_list.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <volk/volk.h>

namespace gr {
  namespace fec {
    namespace code {
      namespace polar {

        scl_list::scl_list(const unsigned int size,
                           const unsigned int block_size,
                           const unsigned int block_power):
          d_list_size(size), d_block_size(block_size), d_block_power(block_power),
          d_num_buff_elements(block_size * (block_power + 1))
        {
          for(unsigned int i = 0; i < 2 * size; i++){
            d_path_list.push_back(new path());
          }

          for(unsigned int i = 0; i < size; i++){
            d_path_list[i]->llr_vec = (float*) volk_malloc(sizeof(float) * d_num_buff_elements, volk_get_alignment());
            memset(d_path_list[i]->llr_vec, 0, sizeof(float) * d_num_buff_elements);
            d_path_list[i]->u_vec = (unsigned char*) volk_malloc(sizeof(unsigned char) * d_num_buff_elements, volk_get_alignment());
            memset(d_path_list[i]->u_vec, 0, sizeof(unsigned char) * d_num_buff_elements);
            d_path_list[i]->owns_vectors = true;
          }

          d_path_list[0]->is_active = true;
          d_active_path_counter = 1;
          d_active_pos = 0;
        }

        scl_list::~scl_list()
        {
          for(unsigned int i = 0; i < d_path_list.size(); i++){
            delete d_path_list[i];
          }
        }


        const path*
        scl_list::optimal_path()
        {
          const path* temp = *std::min_element(d_path_list.begin(), d_path_list.begin() + d_active_path_counter, path_compare);
          reset();
          return temp;
        }

        void
        scl_list::reset()
        {
          // leave 0th element active for next iteration
          d_path_list[0]->path_metric = 0.0f;
          for(unsigned int i = 1; i < d_path_list.size(); i++){
            d_path_list[i]->is_active = false;
            d_path_list[i]->path_metric = 0.0f;
          }
          d_active_path_counter = 1;
          d_active_pos = 0;
        }

        void
        scl_list::set_info_bit(const int bit_pos)
        {
          if(d_active_path_counter < d_list_size) {
            const int offset = d_active_path_counter;
            for(int i = 0; i < offset; i++) {
              duplicate_path(d_path_list[i + offset], d_path_list[i]);
              d_path_list[i]->path_metric = update_path_metric(d_path_list[i]->path_metric,
                                                               d_path_list[i]->llr_vec[bit_pos], 0);
              d_path_list[i + offset]->path_metric = update_path_metric
                (d_path_list[i + offset]->path_metric, d_path_list[i + offset]->llr_vec[bit_pos], 1);
              d_path_list[i]->u_vec[bit_pos] = 0;
              d_path_list[i + offset]->u_vec[bit_pos] = 1;
            }
          }
          else {

            for(unsigned int i = 0; i < d_list_size; i++) {
              branch_paths(d_path_list[i + d_list_size], d_path_list[i], d_path_list[i]->llr_vec[bit_pos]);
            }
            std::sort(d_path_list.begin(), d_path_list.end(), path_compare);

            for(unsigned int i = 0; i < d_list_size; i++) {
              if(!d_path_list[i]->owns_vectors) {
                int t_pos = d_list_size;
                while(!d_path_list[t_pos]->owns_vectors) {
                  t_pos++;
                }
                steal_vector_ownership(d_path_list[i], d_path_list[t_pos]);
                d_path_list[i]->u_vec[bit_pos] = 1;
              }
              else{
                d_path_list[i]->u_vec[bit_pos] = 0;
              }
            }
          }
          d_active_pos = 0;
        }

        void
        scl_list::branch_paths(path* target, path* original, const float llr)
        {
          target->path_metric = update_path_metric(original->path_metric, llr, 1);
          original->path_metric = update_path_metric(original->path_metric, llr, 0);
          target->llr_vec = original->llr_vec;
          target->u_vec = original->u_vec;
        }

        void
        scl_list::steal_vector_ownership(path* target, path* original)
        {
          memcpy(original->llr_vec, target->llr_vec, sizeof(float) * d_num_buff_elements);
          memcpy(original->u_vec, target->u_vec, sizeof(unsigned char) * d_num_buff_elements);
          target->llr_vec = original->llr_vec;
          target->u_vec = original->u_vec;
          target->owns_vectors = true;
          original->owns_vectors = false;
        }

        void
        scl_list::duplicate_path(path* target, const path* original)
        {
          memcpy(target->llr_vec, original->llr_vec, sizeof(float) * d_num_buff_elements);
          memcpy(target->u_vec, original->u_vec, sizeof(unsigned char) * d_num_buff_elements);
          target->path_metric = original->path_metric;
          d_active_path_counter++;
          target->is_active = true;
        }

        float
        scl_list::update_path_metric(const float last_pm, const float llr,
                                     const float ui) const
        {
          if((ui == 0 && llr > 0.0f) || (ui == 1 && llr < 0.0f)){
            //        if(ui == (unsigned char) (0.5 * 1 - copysignf(1.0f, llr))){
            return last_pm;
          }
          return last_pm + fabs(llr);
        }

        void
        scl_list::set_frozen_bit(const unsigned char frozen_bit, const int bit_pos)
        {
          for(unsigned int i = 0; i < d_active_path_counter; i++){
            d_path_list[i]->u_vec[bit_pos] = frozen_bit;
            d_path_list[i]->path_metric = update_path_metric(d_path_list[i]->path_metric,
                                                             d_path_list[i]->llr_vec[bit_pos],
                                                             frozen_bit);
          }
          d_active_pos = 0;
        }

        path::path():
          path_metric(0.0f), owns_vectors(false), is_active(false), llr_vec(NULL), u_vec(NULL)
        {
        }

        path::~path(){
          if(owns_vectors){
            volk_free(llr_vec);
            volk_free(u_vec);
          }
        }

      } /* namespace polar */
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
