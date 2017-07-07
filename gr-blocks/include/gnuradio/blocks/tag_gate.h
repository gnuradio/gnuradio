/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_TAG_GATE_H
#define INCLUDED_BLOCKS_TAG_GATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Control tag propagation.
     * \ingroup blocks
     *
     * Use this block to stop tags from propagating.
     */
    class BLOCKS_API tag_gate : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tag_gate> sptr;

      virtual void set_propagation(bool propagate_tags) = 0;

      /*!
       * \param item_size Item size
       * \param propagate_tags Set this to true to allow tags to pass through this block.
       */
      static sptr make(size_t item_size, bool propagate_tags=false);

      /*!
       * \brief Only gate stream tags with one specific key instead of all keys
       *
       * \details
       * If set to "", all tags will be affected by the gate.
       * If set to "foo", all tags with key different from "foo" will pass
       * through.
       */
      virtual void set_single_key(const std::string &single_key)=0;

      /*!
       * \brief Get the current single key.
      */
      virtual std::string single_key() const = 0;
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_GATE_H */

