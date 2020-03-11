/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_REPETITION_ENCODER_H
#define INCLUDED_FEC_REPETITION_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Repetition Encoding class.
 * \ingroup error_coding_blk
 *
 * \details
 * A repetition encoder class that repeats each input bit \p rep
 * times. To  decode, take  a majority vote  over the  number of
 * repetitions.
 */
class FEC_API repetition_encoder : virtual public generic_encoder
{
public:
    /*!
     * Build a repetition encoding FEC API object.
     *
     * \param frame_size Number of bits per frame. If using in the
     *        tagged stream style, this is the maximum allowable
     *        number of bits per frame.
     * \param rep Repetition rate; encoder rate is rep bits out
     *        for each input bit.
     */
    static generic_encoder::sptr make(int frame_size, int rep);

    /*!
     * Sets the uncoded frame size to \p frame_size. If \p
     * frame_size is greater than the value given to the
     * constructor, the frame size will be capped by that initial
     * value and this function will return false. Otherwise, it
     * returns true.
     */
    virtual bool set_frame_size(unsigned int frame_size) = 0;

    /*!
     * Returns the coding rate of this encoder.
     */
    virtual double rate() = 0;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_REPETITION_ENCODER_H */
