/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011,2012,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <string>

namespace gr {
namespace digital {

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup packet_operators_blk
 *
 * \details
 * input:  stream of floats (generally, soft decisions)
 * output: unaltered stream of bits (plus tags)
 *
 * This block annotates the input stream with tags. The tags have
 * key name [tag_name], specified in the constructor. Used for
 * searching an input data stream for preambles, etc., by slicing
 * the soft decision symbol inputs.
 */
class DIGITAL_API correlate_access_code_tag_ff : virtual public sync_block
{
public:
    // gr::digital::correlate_access_code_tag_ff::sptr
    typedef boost::shared_ptr<correlate_access_code_tag_ff> sptr;

    /*!
     * \param access_code is represented with 1 byte per bit,
     *                    e.g., "010101010111000100"
     * \param threshold maximum number of bits that may be wrong
     * \param tag_name key of the tag inserted into the tag stream
     */
    static sptr
    make(const std::string& access_code, int threshold, const std::string& tag_name);

    /*!
     * \param access_code is represented with 1 byte per bit,
     *                    e.g., "010101010111000100"
     */
    virtual bool set_access_code(const std::string& access_code) = 0;
    virtual void set_threshold(int threshold) = 0;
    virtual void set_tagname(const std::string& tagname) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H */
