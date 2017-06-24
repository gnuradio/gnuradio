/* -*- c++ -*- */
/*
 * Copyright 2014, 2017 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Matrix multiplexer/multiplier: y(k) = A x(k)
     * \ingroup blocks
     *
     * This block is similar to gr::blocks::multiply_const_ff, the difference
     * being it can handle several inputs and outputs, and the input-to-output
     * relation can be described by the following mathematical equation:
     * \f[
     *    \mathbf{y}(k) = \mathbf{A} \mathbf{x}(k) \, , \, y \in \mathbb{R}^N, \mathbf{x} \in \mathbb{R}^M, A \in \mathbb{R}^{N \times M}
     * \f]
     * \f$\mathbf{y}(k)\f$ and \f$\mathbf{x}(i)\f$ are column-vectors describing the elements on the input port
     * at time step \f$k\f$ (this is a sync block with no memory).
     *
     * Examples for where to use this block include:
     * - Switch matrices (i.e. switch which ports go where), assuming all ports run on the same rate
     * - Simulation of static MIMO-Channels (in that case, \f$\mathbf{A}\f$ is the channel matrix)
     * - Summing up streams with variable coefficients
     *
     * This block features a special tag propagation mode: When setting the tag propagation policy
     * to gr::block::TPP_CUSTOM, a tag is propagated from input \f$k\f$
     * to output \f$l\f$, if \f$(A)_{l,k} \neq 0\f$.
     *
     * \section blocks_matrixmult_msgports_@NAME@ Message Ports
     *
     * This block as one input message port (\p set_A). A message sent to this port will
     * be converted to a std::vector<std::vector<@O_TYPE@> >, and then passed on to set_A().
     * If no conversion is possible, a warning is issued via the logging interface, and
     * A remains unchanged.
     *
     * *Note*: It is not possible to change the dimension of the matrix after initialization,
     * as this affects the I/O signature! If a matrix of invalid size is passed to the block,
     * an alert is raised via the logging interface, and A remains unchanged.
     */
    class BLOCKS_API @NAME@ : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<@NAME@> sptr;

      /*!
       * \param A The matrix
       * \param tag_propagation_policy The tag propagation policy.
       *                               Note this can be any
       *                               gr::block::tag_propagation_policy_t
       *                               value. In case of TPP_CUSTOM, tags are
       *                               only transferred from input \f$k\f$ to
       *                               output \f$l \iff (A)_{l,k} \neq 0\f$.
       */
      static sptr make(
          std::vector<std::vector<@O_TYPE@> > A,
          gr::block::tag_propagation_policy_t tag_propagation_policy=gr::block::TPP_ALL_TO_ALL
      );

      //! Returns the current matrix
      virtual const std::vector<std::vector<@O_TYPE@> >& get_A() const = 0;
      //! Sets the matrix to a new value \p new_A. Returns true if the new matrix was valid and could be changed.
      virtual bool set_A(const std::vector<std::vector<@O_TYPE@> > &new_A) = 0;

      static const std::string MSG_PORT_NAME_SET_A;
    };

  } // namespace blocks
} // namespace gr

#endif /* @GUARD_NAME */
