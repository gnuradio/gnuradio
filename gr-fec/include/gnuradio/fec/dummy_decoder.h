/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DUMMY_DECODER_H
#define INCLUDED_FEC_DUMMY_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Dummy Decoding class.
 * \ingroup error_coding_blk
 *
 * \details
 * A dummy decoder class that simply passes the input to the
 * output. It is meant to allow us to easily use the FEC API
 * encoder and decoder blocks in an application with no coding.
 */
class FEC_API dummy_decoder : virtual public generic_decoder
{
public:
    /*!
     * Build a dummy decoding FEC API object.
     *
     * \param frame_size Number of bits per frame. If using in the
     *        tagged stream style, this is the maximum allowable
     *        number of bits per frame.
     */
    static generic_decoder::sptr make(int frame_size);

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

#endif /* INCLUDED_FEC_DUMMY_DECODER_H */
