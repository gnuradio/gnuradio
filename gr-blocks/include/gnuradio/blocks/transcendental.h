/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TRANSCENDENTAL_H
#define INCLUDED_GR_TRANSCENDENTAL_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <string>

namespace gr {
namespace blocks {

/*!
 * \brief A block that performs various transcendental math operations.
 * \ingroup math_operators_blk
 *
 * \details
 * Possible function names can be found in the cmath library. IO
 * may be either complex or real, double or single precision.
 *
 * Possible type strings: float, double, complex_float, complex_double
 *
 * Available functions for real and complex input:
 *
 * - cos
 * - sin
 * - tan
 * - cosh
 * - sinh
 * - tanh
 * - exp
 * - log
 * - log10
 * - sqrt
 *
 * Available functions for real input only:
 *
 * - acos
 * - asin
 * - atan
 *
 * output[i] = trans_fcn(input[i])
 */
class BLOCKS_API transcendental : virtual public sync_block
{
public:
    // gr::blocks::transcendental::sptr
    typedef std::shared_ptr<transcendental> sptr;

    static sptr make(const std::string& name, const std::string& type = "float");
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TRANSCENDENTAL_H */
