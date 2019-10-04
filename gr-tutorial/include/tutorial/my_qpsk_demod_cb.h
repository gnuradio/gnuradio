/* -*- c++ -*- */
/* 
 * Copyright 2019 gr-tutorial author.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
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


#ifndef INCLUDED_TUTORIAL_MY_QPSK_DEMOD_CB_H
#define INCLUDED_TUTORIAL_MY_QPSK_DEMOD_CB_H

#include <tutorial/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace tutorial {

    /*!
     * \brief <+description of block+>
     * \ingroup tutorial
     *
     */
    class TUTORIAL_API my_qpsk_demod_cb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<my_qpsk_demod_cb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of tutorial::my_qpsk_demod_cb.
       *
       * To avoid accidental use of raw pointers, tutorial::my_qpsk_demod_cb's
       * constructor is in a private implementation
       * class. tutorial::my_qpsk_demod_cb::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool gray_code);
    };

  } // namespace tutorial
} // namespace gr

#endif /* INCLUDED_TUTORIAL_MY_QPSK_DEMOD_CB_H */

