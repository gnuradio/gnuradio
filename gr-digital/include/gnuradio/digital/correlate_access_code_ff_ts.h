/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_FF_TS_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_FF_TS_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>
#include <string>

namespace gr {
namespace digital {

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup packet_operators_blk
 *
 * \details
 * input:  stream of floats (generally, soft decisions)
 * output: a tagged stream set of samples from the payload following
 * the access code and header.
 *
 * This block searches for the given access code by slicing the
 * soft decision symbol inputs. Once found, it expects the
 * following 32 samples to contain a header that includes the
 * frame length (16 bits for the length, repeated). It decodes the
 * header to get the frame length in order to set up the the
 * tagged stream key information.
 *
 * The output of this block is appropriate for use with tagged
 * stream blocks.
 */
class DIGITAL_API correlate_access_code_ff_ts : virtual public block
{
public:
    // gr::digital::correlate_access_code_ff_ts::sptr
    typedef boost::shared_ptr<correlate_access_code_ff_ts> sptr;

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
    virtual unsigned long long access_code() const = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_FF_TS_H */
