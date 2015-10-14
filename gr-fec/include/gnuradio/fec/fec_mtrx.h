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

#ifndef INCLUDED_fec_mtrx_H
#define INCLUDED_fec_mtrx_H

#include <gnuradio/fec/api.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace fec {
    namespace code {

      typedef struct
      {
        size_t size;
        double *data;
      } block_data;

      typedef struct
      {
        size_t size1;
        size_t size2;
        size_t tda;
        double * data;
        block_data * block;
        int owner;
      } matrix;

      FEC_API void matrix_free(matrix *x);

      typedef boost::shared_ptr<matrix> matrix_sptr;

      class fec_mtrx;
      typedef boost::shared_ptr<fec_mtrx> fec_mtrx_sptr;

      /*!
       * \brief Read in an alist file and produce the matrix object.
       *
       * \details
       * Takes in a an alist file (the file name as a string) and creates
       * the corresponding matrix. The format of alist files is described
       * at: http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
       *
       * The result is returned as a matrix shared pointer.
       *
       * \param filename Name of an alist file to use. The alist
       *                 format is described at:
       *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
       */
      FEC_API matrix_sptr read_matrix_from_file(const std::string filename);
      FEC_API void write_matrix_to_file(const std::string filename, matrix_sptr M);

      /*!
       * \brief Takes a parity check matrix (H) and returns the
       * transpose of the generator matrix (G).
       *
       * The result is returned as a matrix shared pointer. The form
       * of this matrix is [I_k | P]^T, where P is the parity check
       * matrix. It is a n x k matrix where k is the information
       * length and n is the codeword length.
       *
       * \param H_obj A parity check matrix; generally derived from
       *              using read_matrix_from_file with a given alist
       *              file format.
       */
      FEC_API matrix_sptr generate_G_transpose(matrix_sptr H_obj);

      /*!
       * \brief Takes a parity check matrix (H) and returns the
       * generator matrix (G).
       *
       * The result is returned as a matrix shared pointer. The form
       * of this matrix is [I_k | P], where P is the parity check
       * matrix. It is a k x n matrix where k is the information
       * length and n is the codeword length.
       *
       * \param H_obj A parity check matrix; generally derived from
       *              using read_matrix_from_file with a given alist
       *              file format.
       */
      FEC_API matrix_sptr generate_G(matrix_sptr H_obj);

      /*!
       * \brief Takes a generator matrix (G) and returns the
       * parity check matrix (H).
       *
       * \param G_obj A parity check matrix; generally derived from
       *              using read_matrix_from_file with a given alist
       *              file format.
       */
      FEC_API matrix_sptr generate_H(matrix_sptr G_obj);

      /*!
       * \brief Takes a matrix and prints it to screen.
       *
       * \param M a matrix_sptr; generally a G or H matrix for LDPC codes.
       * \param numpy will output in a format that can be
       *        copy-and-pasted directly into a numpy.matrix(~) call
       *        in Python.
       */
      FEC_API void print_matrix(const matrix_sptr M, bool numpy=false);

      /*!
       * \brief Base class for FEC matrix objects.
       *
       * \ingroup error_coding_blk
       *
       * \details
       *
       * Base class of ldpc_H_matrix and ldpc_G_matrix classes. The
       * child objects can be either generator matrices or parity
       * check matrices. This base class can be provided to the
       * decoder ldpc_bit_flip_decoder, whereas the encoder classes
       * ldpc_gen_mtrx_encoder and ldpc_encoder will not accept this
       * base class; they require one of the child classes.
       */
      class FEC_API fec_mtrx
      {
      protected:
        fec_mtrx(void) {} // allows pure virtual interface sub-classes

      public:
        virtual ~fec_mtrx() {}

        //! Encode \p inbuffer with LDPC H matrix into \p outbuffer.
        virtual void encode(unsigned char *outbuffer,
                            const unsigned char *inbuffer) const = 0;

        //! Decode \p inbuffer with LDPC H matrix into \p outbuffer.
        virtual void decode(unsigned char *outbuffer,
                            const float *inbuffer,
                            unsigned int frame_size,
                            unsigned int max_iterations) const = 0;

        //!Get the codeword length n
        virtual unsigned int n() const = 0;

        //! Get the information word length k
        virtual unsigned int k() const = 0;
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_fec_mtrx_H */
