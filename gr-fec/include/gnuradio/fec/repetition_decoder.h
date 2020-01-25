/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_REPETITION_DECODER_H
#define INCLUDED_FEC_REPETITION_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Repetition Decoding class.
 * \ingroup error_coding_blk
 *
 * \details
 * A repetition decoder class. This takes a majority vote,
 * biased by the \p ap_prob rate, and decides if the number of 1
 * bits > ap_prob, it is a 1; else, it is a 0.
 */
class FEC_API repetition_decoder : virtual public generic_decoder
{
public:
    /*!
     * Build a repetition decoding FEC API object.
     *
     * \param frame_size Number of bits per frame. If using in the
     *        tagged stream style, this is the maximum allowable
     *        number of bits per frame.
     * \param rep Repetition rate; encoder rate is rep bits out
     *        for each input bit.
     * \param ap_prob The a priori probability that a bit is a 1
     *        (generally, unless otherwise known, assume to be
     *        0.5).
     */
    static generic_decoder::sptr make(int frame_size, int rep, float ap_prob = 0.5);

    /*!
     * Sets the uncoded frame size to \p frame_size. If \p
     * frame_size is greater than the value given to the
     * constructor, the frame size will be capped by that initial
     * value and this function will return false. Otherwise, it
     * returns true.
     */
    virtual bool set_frame_size(unsigned int frame_size) = 0;

    /*!
     * Returns the coding rate of this encoder (it will always be 1).
     */
    virtual double rate() = 0;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_REPETITION_DECODER_H */
