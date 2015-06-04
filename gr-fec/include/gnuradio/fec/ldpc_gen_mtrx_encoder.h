/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation; either version 3, or (at your 
 * option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H
#define INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/ldpc_HorG_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief LDPC Generic Encoder (method by Richardson & Urbanke)
       * \ingroup error_coding_blk
       *
       * \details
       * A standard encoder class. This method is discussed in many
         textbooks. One is: Turbo Coding for Satellite and Wireless
         Communications by Soleymani, Gao, and Vilaipornsawai.
         The generator matrix G must be in systematic form, 
         G = [I P], where I is an identity matrix and P is the
         parity submatrix.
       */
      class FEC_API ldpc_gen_mtrx_encoder : virtual public generic_encoder
      {
      public:
        /*!
         * Build an encoding FEC API object.
         *
         * \param G_obj The generator matrix object to use
         *        for encoding.
        */
        static generic_encoder::sptr make(const ldpc_HorG_mtrx *M_obj);

        /*!
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        /*!
         * Returns the coding rate of this encoder.
         */
        virtual double rate() = 0;
      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H */