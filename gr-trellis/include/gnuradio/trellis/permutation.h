/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_PERMUTATION_H
#define INCLUDED_TRELLIS_PERMUTATION_H

#include <gnuradio/sync_block.h>
#include <gnuradio/trellis/api.h>
#include <vector>

namespace gr {
namespace trellis {

/*!
 * \brief Permutation.
 * \ingroup trellis_coding_blk
 */
class TRELLIS_API permutation : virtual public sync_block
{
public:
    // gr::trellis::permutation::sptr
    typedef boost::shared_ptr<permutation> sptr;

    static sptr
    make(int K, const std::vector<int>& TABLE, int SYMS_PER_BLOCK, size_t NBYTES);

    virtual int K() const = 0;
    virtual std::vector<int> TABLE() const = 0;
    virtual int SYMS_PER_BLOCK() const = 0;
    virtual size_t BYTES_PER_SYMBOL() const = 0;

    virtual void set_K(int K) = 0;
    virtual void set_TABLE(const std::vector<int>& table) = 0;
    virtual void set_SYMS_PER_BLOCK(int spb) = 0;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_PERMUTATION_H */
