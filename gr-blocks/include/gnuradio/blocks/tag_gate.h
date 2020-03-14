/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    typedef std::shared_ptr<tag_gate> sptr;

    virtual void set_propagation(bool propagate_tags) = 0;

    /*!
     * \param item_size Item size
     * \param propagate_tags Set this to true to allow tags to pass through this block.
     */
    static sptr make(size_t item_size, bool propagate_tags = false);

    /*!
     * \brief Only gate stream tags with one specific key instead of all keys
     *
     * \details
     * If set to "", all tags will be affected by the gate.
     * If set to "foo", all tags with key different from "foo" will pass
     * through.
     */
    virtual void set_single_key(const std::string& single_key) = 0;

    /*!
     * \brief Get the current single key.
     */
    virtual std::string single_key() const = 0;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_GATE_H */
