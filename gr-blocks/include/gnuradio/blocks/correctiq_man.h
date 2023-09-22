/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_MAN_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_MAN_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief This block provides a mechanism to manually provide a real and
 * \p imaginary signal offset.  Very similar to a complex add block,
 * \p the block supports dynamic updating on the values.
 * \ingroup iq_correction
 *
 */
class BLOCKS_API correctiq_man : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<correctiq_man> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of correctiq::correctiq_man.
     *
     * To avoid accidental use of raw pointers, correctiq::correctiq_man's
     * constructor is in a private implementation
     * class. correctiq::correctiq_man::make is the public interface for
     * creating new instances.
     */

    virtual float get_real() const = 0;
    virtual float get_imag() const = 0;

    virtual void set_real(float newValue) = 0;
    virtual void set_imag(float newValue) = 0;

    static sptr make(float real, float imag);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_MAN_H */
