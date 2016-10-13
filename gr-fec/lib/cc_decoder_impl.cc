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

#include "cc_decoder_impl.h"
#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>

namespace gr {
  namespace fec {
    namespace code {

      generic_decoder::sptr
      cc_decoder::make(int frame_size, int k,
                       int rate, std::vector<int> polys,
                       int start_state, int end_state,
                       cc_mode_t mode, bool padded)
      {
        return generic_decoder::sptr
          (new cc_decoder_impl(frame_size, k, rate, polys,
                               start_state, end_state, mode, padded));
      }

      cc_decoder_impl::cc_decoder_impl(int frame_size, int k,
                                       int rate, std::vector<int> polys,
                                       int start_state, int end_state,
                                       cc_mode_t mode, bool padded)
        : generic_decoder("cc_decoder"),
          d_k(k),
          d_rate(rate),
          d_partial_rate(rate),
          d_polys(polys),
          d_mode(mode),
          d_padding(0),
          d_start_state_chaining(start_state),
          d_start_state_nonchaining(start_state),
          d_end_state_nonchaining(end_state)
      {
        // Set max frame size here; all buffers and settings will be
        // based on this value.
        d_max_frame_size = frame_size;
        d_frame_size = frame_size;

        // set up a padding factor. If padding, the encoded frame was exteded
        // by this many bits to fit into a full byte.
        if(padded && (mode == CC_TERMINATED)) {
          d_padding = static_cast<int>(8.0f*ceilf(d_rate*(d_k-1)/8.0f) - (d_rate*(d_k-1)));
        }

        d_vp = new struct v;

        d_numstates = 1 << (d_k - 1);

        d_decision_t_size = d_numstates/8; //packed bit array

        d_managed_in_size = 0;
        switch(d_mode) {
        case(CC_TAILBITING):
          d_end_state = &d_end_state_chaining;
          d_veclen = d_frame_size + (6 * (d_k - 1));
          d_managed_in = (unsigned char*)volk_malloc(d_veclen*d_rate*sizeof(unsigned char),
                                                     volk_get_alignment());
          d_managed_in_size = d_veclen * d_rate;
          if(d_managed_in == NULL) {
            throw std::runtime_error("cc_decoder: bad alloc for d_managed_in\n");
          }
          break;

        case(CC_TRUNCATED):
          d_veclen = d_frame_size;
          d_end_state = &d_end_state_chaining;
          break;

        case(CC_TERMINATED):
          d_veclen = d_frame_size + d_k - 1;
          d_end_state = (end_state == -1) ? &d_end_state_chaining : &d_end_state_nonchaining;
          break;

        case(CC_STREAMING):
          d_veclen = d_frame_size + d_k - 1;
          d_end_state = &d_end_state_chaining;
          break;

        default:
          throw std::runtime_error("cc_decoder: mode not recognized");
        }

        d_vp->metrics = (unsigned char*)volk_malloc(2*sizeof(unsigned char)*d_numstates,
                                                    volk_get_alignment());
        if(d_vp->metrics == NULL) {
          throw std::runtime_error("bad alloc for d_vp->metrics!\n");
        }

        d_vp->metrics1.t = d_vp->metrics;
        d_vp->metrics2.t = d_vp->metrics + d_numstates;

        d_vp->decisions = (unsigned char*)volk_malloc(sizeof(unsigned char)*d_veclen*d_decision_t_size,
                                                      volk_get_alignment());
        if(d_vp->decisions == NULL) {
          throw std::runtime_error("bad alloc for d_vp->decisions!\n");
        }

        Branchtab = (unsigned char*)volk_malloc(sizeof(unsigned char)*d_numstates/2*rate,
                                                volk_get_alignment());
        if(Branchtab == NULL) {
          throw std::runtime_error("bad alloc for d_vp->decisions!\n");
        }

        create_viterbi();

        if(d_k-1<8) {
          d_ADDSHIFT = (8-(d_k-1));
          d_SUBSHIFT =  0;
        }
        else if(d_k-1>8) {
          d_ADDSHIFT = 0;
          d_SUBSHIFT =  ((d_k-1)-8);
        }
        else {
          d_ADDSHIFT = 0;
          d_SUBSHIFT = 0;
        }

        std::map<std::string, conv_kernel> yp_kernel = boost::assign::map_list_of("k=7r=2", volk_8u_x4_conv_k7_r2_8u);

        std::string k_ = "k=";
        std::string r_ = "r=";

        std::ostringstream kerneltype;
        kerneltype << k_ << d_k << r_ << d_rate;

        d_kernel = yp_kernel[kerneltype.str()];
        if (d_kernel == NULL) {
          throw std::runtime_error("cc_decoder: parameters not supported");
        }
      }

      cc_decoder_impl::~cc_decoder_impl()
      {
        volk_free(d_vp->decisions);
        volk_free(Branchtab);
        volk_free(d_vp->metrics);

	delete d_vp;

	if(d_mode == CC_TAILBITING) {
	  volk_free(d_managed_in);
	}
      }

      int
      cc_decoder_impl::get_output_size()
      {
        //unpacked bits
        return d_frame_size;
      }

      int
      cc_decoder_impl::get_input_size()
      {
        if(d_mode == CC_TERMINATED) {
          return d_rate * (d_frame_size + d_k - 1) + d_padding;
        }
        else {
          return d_rate * d_frame_size;
        }
      }

      int
      cc_decoder_impl::get_input_item_size()
      {
        return 1;
      }

      int
      cc_decoder_impl::get_history()
      {
        if(d_mode == CC_STREAMING) {
          return d_rate * (d_k - 1);
        }
        else {
          return 0;
        }
      }

      float
      cc_decoder_impl::get_shift()
      {
        return 128.0;
      }

      const char*
      cc_decoder_impl::get_input_conversion()
      {
        return "uchar";
      }

      void
      cc_decoder_impl::create_viterbi()
      {
        int state;
        unsigned int i;
        partab_init();
        for(state = 0; state < d_numstates/2; state++) {
          for(i = 0; i < d_rate; i++) {
            Branchtab[i*d_numstates/2+state] = (d_polys[i] < 0) ^ parity((2*state) & abs(d_polys[i])) ? 255 : 0;
          }
        }

        switch(d_mode) {
        case(CC_STREAMING):
          d_start_state = &d_start_state_chaining;
          init_viterbi_unbiased(d_vp);
          break;

        case(CC_TAILBITING):
          d_start_state = &d_start_state_nonchaining;
          init_viterbi_unbiased(d_vp);
          break;

        case(CC_TRUNCATED):
        case(CC_TERMINATED):
          d_start_state = &d_start_state_nonchaining;
          init_viterbi(d_vp, *d_start_state);
          break;

        default:
          throw std::runtime_error("cc_decoder: mode not recognized");
        }

        return;
      }

      int
      cc_decoder_impl::parity(int x)
      {
        x ^= (x >> 16);
        x ^= (x >> 8);
        return parityb(x);
      }

      int
      cc_decoder_impl::parityb(unsigned char x)
      {
        return Partab[x];
      }

      void
      cc_decoder_impl::partab_init(void)
      {
        int i,cnt,ti;

        /* Initialize parity lookup table */
        for(i=0;i<256;i++){
          cnt = 0;
          ti = i;
          while(ti){
            if(ti & 1)
              cnt++;
            ti >>= 1;
          }
          Partab[i] = cnt & 1;
        }
      }

      int
      cc_decoder_impl::init_viterbi(struct v* vp, int starting_state)
      {
        int i;

        if(vp == NULL)
          return -1;
        for(i = 0; i < d_numstates; i++) {
          vp->metrics1.t[i] = 63;
        }

        vp->old_metrics = vp->metrics1;
        vp->new_metrics = vp->metrics2;
        vp->old_metrics.t[starting_state & (d_numstates-1)] = 0; /* Bias known start state */
        return 0;
      }

      int
      cc_decoder_impl::init_viterbi_unbiased(struct v* vp)
      {
        int i;

        if(vp == NULL)
          return -1;
        for(i=0;i<d_numstates;i++)
          vp->metrics1.t[i] = 31;

        vp->old_metrics = vp->metrics1;
        vp->new_metrics = vp->metrics2;
        //no bias step
        return 0;
      }

      int
      cc_decoder_impl::find_endstate()
      {
        unsigned char* met = ((d_k + d_veclen)%2 == 0)? d_vp->new_metrics.t : d_vp->old_metrics.t;

        unsigned char min = met[0];
        int state = 0;
        for(int i = 1; i < d_numstates; ++i) {
          if(met[i] < min) {
            min = met[i];
            state = i;
          }


        }
        //printf("min %d\n", state);
        return state;
      }

      int
      cc_decoder_impl::update_viterbi_blk(unsigned char* syms, int nbits)
      {
        unsigned char *d;

        d = d_vp->decisions;

        memset(d,0,d_decision_t_size * nbits);

        d_kernel(d_vp->new_metrics.t, d_vp->old_metrics.t, syms,
                 d, nbits - (d_k - 1), d_k - 1, Branchtab);

        return 0;
      }

      int
      cc_decoder_impl::chainback_viterbi(unsigned char* data,
                                         unsigned int nbits,
                                         unsigned int endstate,
                                         unsigned int tailsize)
      {
        unsigned char *d;

        /* ADDSHIFT and SUBSHIFT make sure that the thing returned is a byte. */
        d = d_vp->decisions;
        /* Make room beyond the end of the encoder register so we can
         * accumulate a full byte of decoded data
         */

        endstate = (endstate%d_numstates) << d_ADDSHIFT;

        /* The store into data[] only needs to be done every 8 bits.
         * But this avoids a conditional branch, and the writes will
         * combine in the cache anyway
         */

        d += tailsize * d_decision_t_size ; /* Look past tail */
        int retval;
        int dif = tailsize - (d_k - 1);
        decision_t dec;
        while(nbits-- > d_frame_size - (d_k - 1)) {
          int k;
          dec.t = &d[nbits * d_decision_t_size];
          k = (dec.w[(endstate>>d_ADDSHIFT)/32] >> ((endstate>>d_ADDSHIFT)%32)) & 1;

          endstate = (endstate >> 1) | (k << (d_k-2+d_ADDSHIFT));
          data[((nbits+dif)%d_frame_size)] = k;

          retval = endstate;
        }
        nbits += 1;

        while(nbits-- != 0) {
          int k;

          dec.t = &d[nbits * d_decision_t_size];

          k = (dec.w[(endstate>>d_ADDSHIFT)/32] >> ((endstate>>d_ADDSHIFT)%32)) & 1;

          endstate = (endstate >> 1) | (k << (d_k-2+d_ADDSHIFT));
          data[((nbits+dif)%d_frame_size)] = k;
        }

        return retval >> d_ADDSHIFT;
      }

      bool
      cc_decoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("cc_decoder: tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }

        d_frame_size = frame_size;

        switch(d_mode) {
        case(CC_TAILBITING):
          d_veclen = d_frame_size + (6 * (d_k - 1));
          if(d_veclen * d_rate > d_managed_in_size) {
            throw std::runtime_error("cc_decoder: attempt to resize beyond d_managed_in buffer size!\n");
          }
          break;

        case(CC_TRUNCATED):
          d_veclen = d_frame_size;
          break;

        case(CC_STREAMING):
          d_veclen = d_frame_size + d_k - 1;
          break;

        case(CC_TERMINATED):
          // If the input is being padded out to a byte, we know the
          // real frame size is without the padding.
          d_frame_size -= d_padding * d_rate;
          d_veclen = d_frame_size + d_k - 1;
          break;

        default:
          throw std::runtime_error("cc_decoder: mode not recognized");
        }

        return ret;
      }

      double
      cc_decoder_impl::rate()
      {
        return 1.0/static_cast<double>(d_rate);
      }

      void
      cc_decoder_impl::generic_work(void *inbuffer, void *outbuffer)
      {
        const unsigned char *in = (const unsigned char *) inbuffer;
        unsigned char *out = (unsigned char *) outbuffer;

        switch(d_mode) {

        case(CC_TAILBITING):
          memcpy(d_managed_in, in, d_frame_size * d_rate * sizeof(unsigned char));
          memcpy(d_managed_in + d_frame_size * d_rate * sizeof(unsigned char), in,
                 (d_veclen - d_frame_size) * d_rate * sizeof(unsigned char));
          update_viterbi_blk(d_managed_in, d_veclen);
          d_end_state_chaining = find_endstate();
          chainback_viterbi(&out[0], d_frame_size, *d_end_state, d_veclen - d_frame_size);
          init_viterbi_unbiased(d_vp);
          break;


        case(CC_TRUNCATED):
          update_viterbi_blk((unsigned char*)(&in[0]), d_veclen);
          d_end_state_chaining = find_endstate();
          for(unsigned int i = 0; i < d_k-1; ++i) {
            out[d_veclen - 1 - i] = ((*d_end_state) >> i) & 1;
          }
          d_start_state_chaining = chainback_viterbi(&out[0], d_frame_size - (d_k - 1),
                                                     *d_end_state, d_k - 1);
          init_viterbi(d_vp, *d_start_state);
          break;

        case(CC_STREAMING):
        case(CC_TERMINATED):
          update_viterbi_blk((unsigned char*)(&in[0]), d_veclen);
          d_end_state_chaining = find_endstate();
          d_start_state_chaining = chainback_viterbi(&out[0], d_frame_size, *d_end_state,
                                                     d_veclen - d_frame_size);

          init_viterbi(d_vp, *d_start_state);
          break;

        default:
          throw std::runtime_error("cc_decoder: mode not recognized");
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
