/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_BLOCKS_LAMBDA_BLOCK_H
#define INCLUDED_BLOCKS_LAMBDA_BLOCK_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <gnuradio/io_signature.h>
#include <functional>

namespace gr {
namespace blocks {

/*!
 * \brief Map a generic function object or lambda into a GNU Radio block
 * \ingroup misc_blk
 *
 * This block takes a function object as an argument, and uses that to execute
 * the general_work() function.
 *
 * A neat use case for this block is for hierarchical blocks which require a
 * kernel of custom processing; using the lambda block they can define their
 * own partial work function.
 */
class BLOCKS_API lambda_block : virtual public block
{
public:
    // gr::blocks::delay::sptr
    typedef std::shared_ptr<lambda_block> sptr;

    using general_work_func_t = std::function<int(lambda_block*,
                                                  int,
                                                  gr_vector_int&,
                                                  gr_vector_const_void_star&,
                                                  gr_vector_void_star&)>;

    /*!
     * \brief Make a delay block.
     * \param itemsize size of the data items.
     * \param delay number of samples to delay stream (>= 0).
     */
    static sptr make(general_work_func_t&& work_func,
                     io_signature::sptr in,
                     io_signature::sptr out,
                     const std::string& name = "lambda_block");


    inline void add_item_tag(unsigned int which_output,
                             uint64_t abs_offset,
                             const pmt::pmt_t& key,
                             const pmt::pmt_t& value,
                             const pmt::pmt_t& srcid = pmt::PMT_F)
    {
        gr::block::add_item_tag(which_output, abs_offset, key, value, srcid);
    }

    inline void add_item_tag(unsigned int which_output, const tag_t& tag)
    {
        gr::block::add_item_tag(which_output, tag);
    }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_LAMBDA_BLOCK_H */
