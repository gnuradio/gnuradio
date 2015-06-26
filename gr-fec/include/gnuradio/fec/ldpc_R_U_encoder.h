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

#ifndef INCLUDED_FEC_LDPC_R_U_ENCODER_H
#define INCLUDED_FEC_LDPC_R_U_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/ldpc_R_U_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief LDPC generic encoder (method by Richardson & Urbanke)
       * \ingroup error_coding_blk
       *
       * \details
       * A generic LDPC encoder class. This encoding method is
       * described by Richardson and Urbanke in Appendix A of their
       * book Modern Coding Theory (ISBN 978-0-521-85229-6).
       *
       * Summary of the steps:
       * 1. Let \f$\overline{s}\f$ be the information word to be
       *    encoded.
       * 2. Define \f$\overline{p}_{1}^{T}=T\left[A\overline{p}_{2}^{T}+B\overline{s}^{T}\right]\f$
       * 3. Define \f$\overline{p}_{2}^{T}=-\phi^{-1}\left[D-ET^{-1}B\right]\overline{s}^{T}\f$
       * 4. Taking advantage of \f$T\f$ being square and upper
       *    triangular, use back substitution to solve for
       *    \f$\overline{p}_{1}\f$ and \f$\overline{p}_{2}\f$.
       * 5. The codeword is given by: \f$\overline{x}=\left[\overline{p}_{1},\overline{p}_{2},\overline{s}\right]\f$
       */
      class FEC_API ldpc_R_U_encoder : virtual public generic_encoder
      {
      public:
        /*!
         * \brief Build an encoding FEC API object.
         * \param H_obj The ldpc_R_U_mtrx object to use for
         *              encoding. The decoder must be given this
         *              same matrix object of course.
        */
        static generic_encoder::sptr make(const ldpc_R_U_mtrx *H_obj);

        /*!
         * \brief  Sets the uncoded frame size to \p frame_size.
         * \details
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        //! Returns the coding rate of this decoder.
        virtual double rate() = 0;
      };
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_R_U_ENCODER_H */