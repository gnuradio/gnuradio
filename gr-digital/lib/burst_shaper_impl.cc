/* -*- c++ -*- */
/*
 * Copyright 2015,2018 Free Software Foundation, Inc.
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

#include "burst_shaper_impl.h"
#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace digital {

    template <class T>
    typename burst_shaper<T>::sptr
    burst_shaper<T>::make(const std::vector<T> &taps,
                      int pre_padding, int post_padding,
                      bool insert_phasing,
                      const std::string &length_tag_name)
    {
      return gnuradio::get_initial_sptr
        (new burst_shaper_impl<T>(taps, pre_padding, post_padding,
                         insert_phasing, length_tag_name));
    }

    template <class T>
    burst_shaper_impl<T>::burst_shaper_impl(const std::vector<T> &taps,
                             int pre_padding, int post_padding,
                             bool insert_phasing,
                             const std::string &length_tag_name)
      : gr::block("burst_shaper",
              gr::io_signature::make(1, 1, sizeof(T)),
              gr::io_signature::make(1, 1, sizeof(T))),
        d_up_ramp(taps.begin(), taps.begin() + taps.size()/2 + taps.size()%2),
        d_down_ramp(taps.begin() + taps.size()/2, taps.end()),
        d_nprepad(pre_padding),
        d_npostpad(post_padding),
        d_insert_phasing(insert_phasing),
        d_length_tag_key(pmt::string_to_symbol(length_tag_name)),
        d_ncopy(0),
        d_limit(0),
        d_index(0),
        d_length_tag_offset(0),
        d_finished(false),
        d_state(STATE_WAIT)
    {
        assert(d_up_ramp.size() == d_down_ramp.size());

        d_up_phasing.resize(d_up_ramp.size());
        d_down_phasing.resize(d_down_ramp.size());

        T symbol;
        for(unsigned int i = 0; i < d_up_ramp.size(); i++) {
            symbol = (i%2 == 0) ? T(1.0f) : T(-1.0f);
            d_up_phasing[i] = symbol * d_up_ramp[i];
            d_down_phasing[i] = symbol * d_down_ramp[i];
        }

        //this->set_relative_rate(1, 1);
        this->set_tag_propagation_policy(gr::block::TPP_DONT);
    }

    template <class T>
    burst_shaper_impl<T>::~burst_shaper_impl()
    {
    }

    template <class T>
    void
    burst_shaper_impl<T>::forecast(int noutput_items,
                          gr_vector_int &ninput_items_required)
    {
      switch(d_state) {
      case(STATE_RAMPDOWN):
        // If inserting phasing; we don't need any input
        if(d_insert_phasing) {
          ninput_items_required[0] = 0;
        }
        else {
          ninput_items_required[0] = noutput_items;
        }
        break;
      case(STATE_POSTPAD):
        // Padding 0's requires no input
        ninput_items_required[0] = 0;
        break;
      default:
        ninput_items_required[0] = noutput_items;
      }
    }

    template <class T>
    int
    burst_shaper_impl<T>::general_work(int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
        const T *in = reinterpret_cast<const T *>(input_items[0]);
        T *out = reinterpret_cast<T *>(output_items[0]);

        int nwritten = 0;
        int nread = 0;
        int nspace = 0;
        int nskip = 0;
        int curr_tag_index = 0;

        std::vector<tag_t> length_tags;
        this->get_tags_in_window(length_tags, 0, 0, ninput_items[0], d_length_tag_key);
        std::sort(length_tags.rbegin(), length_tags.rend(), tag_t::offset_compare);

        while(nwritten < noutput_items) {
            // Only check the nread condition if we are actually reading
            // from the input stream.
            if(((d_state != STATE_RAMPDOWN) && (d_state != STATE_POSTPAD)) ||
               ((d_state == STATE_RAMPDOWN) && !d_insert_phasing))
              {
                if(nread >= ninput_items[0]) {
                    break;
                }
            }

            if(d_finished) {
                d_finished = false;
                break;
            }
            nspace = noutput_items - nwritten;
            switch(d_state) {
                case(STATE_WAIT):
                    if(!length_tags.empty()) {
                        d_length_tag_offset = length_tags.back().offset;
                        curr_tag_index = (int)(d_length_tag_offset - this->nitems_read(0));
                        d_ncopy = pmt::to_long(length_tags.back().value);
                        length_tags.pop_back();
                        nskip = curr_tag_index - nread;
                        add_length_tag(nwritten);
                        propagate_tags(curr_tag_index, nwritten, 1, false);
                        enter_prepad();
                    }
                    else {
                        nskip = ninput_items[0] - nread;
                    }
                    if(nskip > 0) {
                        GR_LOG_WARN(this->d_logger,
                                    boost::format("Dropping %1% samples") %
                                    nskip);
                        nread += nskip;
                        in += nskip;
                    }
                    break;

                case(STATE_PREPAD):
                    write_padding(out, nwritten, nspace);
                    if(d_index == d_limit)
                        enter_rampup();
                    break;

                case(STATE_RAMPUP):
                    apply_ramp(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_copy();
                    break;

                case(STATE_COPY):
                    copy_items(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_rampdown();
                    break;

                case(STATE_RAMPDOWN):
                    apply_ramp(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_postpad();
                    break;

                case(STATE_POSTPAD):
                    write_padding(out, nwritten, nspace);
                    if(d_index == d_limit)
                        enter_wait();
                    break;

                default:
                    throw std::runtime_error("burst_shaper: invalid state");
            }
        }

        this->consume_each(nread);

        return nwritten;
    }

    template <class T>
    int
    burst_shaper_impl<T>::prefix_length() const
    {
        return (d_insert_phasing) ?
               d_nprepad + d_up_ramp.size() : d_nprepad;
    }

      template <class T>
    int
    burst_shaper_impl<T>::suffix_length() const
    {
        return (d_insert_phasing) ?
               d_npostpad + d_down_ramp.size() : d_npostpad;
    }

      template <class T>
    void
    burst_shaper_impl<T>::write_padding(T *&dst, int &nwritten, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        std::memset(dst, 0x00, nprocess * sizeof(T));
        dst += nprocess;
        nwritten += nprocess;
        d_index += nprocess;
    }

      template <class T>
    void
    burst_shaper_impl<T>::copy_items(T *&dst, const T *&src, int &nwritten,
                            int &nread, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        propagate_tags(nread, nwritten, nprocess);
        std::memcpy(dst, src, nprocess * sizeof(T));
        dst += nprocess;
        nwritten += nprocess;
        src += nprocess;
        nread += nprocess;
        d_index += nprocess;
    }

      template <>
    void
    burst_shaper_impl<gr_complex>::apply_ramp(gr_complex *&dst, const gr_complex *&src, int &nwritten,
                            int &nread, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        gr_complex *phasing;
        const gr_complex *ramp;

        if(d_state == STATE_RAMPUP) {
            phasing = &d_up_phasing[d_index];
            ramp = &d_up_ramp[d_index];
        }
        else {
            phasing = &d_down_phasing[d_index];
            ramp = &d_down_ramp[d_index];
        }

        if(d_insert_phasing)
            std::memcpy(dst, phasing, nprocess * sizeof(gr_complex));
        else {
            propagate_tags(nread, nwritten, nprocess);
            volk_32fc_x2_multiply_32fc(dst, src, ramp, nprocess);
            src += nprocess;
            nread += nprocess;
        }

        dst += nprocess;
        nwritten += nprocess;
        d_index += nprocess;
    }

      template <>
      void
      burst_shaper_impl<float>::apply_ramp(float *&dst, const float *&src, int &nwritten,
                                                int &nread, int nspace)
      {
          int nprocess = std::min(d_limit - d_index, nspace);
          float *phasing;
          const float *ramp;

          if(d_state == STATE_RAMPUP) {
              phasing = &d_up_phasing[d_index];
              ramp = &d_up_ramp[d_index];
          }
          else {
              phasing = &d_down_phasing[d_index];
              ramp = &d_down_ramp[d_index];
          }

          if(d_insert_phasing)
              std::memcpy(dst, phasing, nprocess * sizeof(float));
          else {
              propagate_tags(nread, nwritten, nprocess);
              volk_32f_x2_multiply_32f(dst, src, ramp, nprocess);
              src += nprocess;
              nread += nprocess;
          }

          dst += nprocess;
          nwritten += nprocess;
          d_index += nprocess;
      }



      template <class T>
    void
    burst_shaper_impl<T>::add_length_tag(int offset)
    {
        this->add_item_tag(0, this->nitems_written(0) + offset, d_length_tag_key,
                     pmt::from_long(d_ncopy + prefix_length() +
                                    suffix_length()),
                     pmt::string_to_symbol(this->name()));
    }

      template <class T>
    void
    burst_shaper_impl<T>::propagate_tags(int in_offset, int out_offset, int count, bool skip)
    {
        uint64_t abs_start = this->nitems_read(0) + in_offset;
        uint64_t abs_end = abs_start + count;
        uint64_t abs_offset = this->nitems_written(0) + out_offset;
        tag_t temp_tag;

        std::vector<tag_t> tags;
        std::vector<tag_t>::iterator it;

        this->get_tags_in_range(tags, 0, abs_start, abs_end);

        for(it = tags.begin(); it != tags.end(); it++) {
            if(!pmt::equal(it->key, d_length_tag_key)) {
                if(skip && (it->offset == d_length_tag_offset))
                    continue;
                temp_tag = *it;
                temp_tag.offset = abs_offset + it->offset - abs_start;
                this->add_item_tag(0, temp_tag);
            }
        }
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_wait()
    {
        d_finished = true;
        d_index = 0;
        d_state = STATE_WAIT;
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_prepad()
    {
        d_limit = d_nprepad;
        d_index = 0;
        d_state = STATE_PREPAD;
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_rampup()
    {
        if(d_insert_phasing)
            d_limit = d_up_ramp.size();
        else
            d_limit = std::min((size_t)(d_ncopy/2), d_up_ramp.size());
        d_index = 0;
        d_state = STATE_RAMPUP;
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_copy()
    {
        if(d_insert_phasing)
            d_limit = d_ncopy;
        else
            d_limit = d_ncopy - std::min((size_t)((d_ncopy/2)*2),
                                         d_up_ramp.size() +
                                         d_down_ramp.size());
        d_index = 0;
        d_state = STATE_COPY;
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_rampdown()
    {
        if(d_insert_phasing)
            d_limit = d_down_ramp.size();
        else
            d_limit = std::min((size_t)(d_ncopy/2), d_down_ramp.size());
        d_index = 0;
        d_state = STATE_RAMPDOWN;
    }

      template <class T>
    void
    burst_shaper_impl<T>::enter_postpad()
    {
        d_limit = d_npostpad;
        d_index = 0;
        d_state = STATE_POSTPAD;
    }

      template class burst_shaper<gr_complex>;
      template class burst_shaper<float>;
  } /* namespace digital */
} /* namespace gr */
