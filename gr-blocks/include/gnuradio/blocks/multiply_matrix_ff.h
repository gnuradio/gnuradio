/* -*- c++ -*- */
/* 
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_H
#define INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Matrix multiplexer/multiplier: y(k) = A * x(k)
     * \ingroup blocks
     *
     * This block is similar to gr::blocks::multiply_const_ff, the difference
     * being it can handle several inputs and outputs, and the input-to-output
     * relation can be described by the following mathematical equation:
     * \[
     *    y(k) = A x(k) \, , \, y \in \mathbb{R}^N, x \in \mathbb{R}^M, A \in \mathbb{R}^{N \times M}
     * \]
     * \$y(k)\$ and \$x(i)\$ are column-vectors describing the elements on the input port
     * at time step \$k\$ (this is a sync block with no memory).
     *
     * Examples for where to use this block include:
     * - Switch matrices (i.e. switch which ports go where)
     * - Simulation of static MIMO-Channels (in that case, \$A\$ is the channel matrix)
     *
     * This block features a special tag propagation mode: When setting the tag propagation policy
     * to gr::blocks::multiply_matrix_ff::TPP_SELECT_BY_MATRIX, a tag is propagated from input k
     * to output l if \$(A)_{l,k} \neq 0\$.
     *
     * A message port (\p set_A) allows to set the matrix. *Note*: It is not possible to change
     * the dimension of the matrix after initialization, as this affects the I/O signature!
     */
    class BLOCKS_API multiply_matrix_ff : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<multiply_matrix_ff> sptr;

      /*!
       * \param A The matrix
       * \param tag_propagation_policy The tag propagation policy.
       *                               Note this can be any gr::block::tag_propagation_policy_t value, or TPP_SELECT_BY_MATRIX.
       */
      static sptr make(
          std::vector<std::vector<float> > A,
          gr::block::tag_propagation_policy_t tag_propagation_policy=gr::block::TPP_ALL_TO_ALL
      );

      //! Returns the current matrix
      virtual const std::vector<std::vector<float> >& get_A() const = 0;
      //! Sets the matrix to a new value \p new_A. Returns true if the new matrix was valid and could be changed.
      virtual bool set_A(const std::vector<std::vector<float> > &new_A) = 0;

      /*!
       * \brief Set the policy by the scheduler to determine how tags are moved downstream.
       *
       * This will also accept the value TPP_SELECT_BY_MATRIX.
       */
      virtual void set_tag_propagation_policy(gr::block::tag_propagation_policy_t p) = 0;

      static const int TPP_SELECT_BY_MATRIX = 999;
      static const std::string MSG_PORT_NAME_SET_A;
    };
    const std::string multiply_matrix_ff::MSG_PORT_NAME_SET_A = "set_A";

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_H */

