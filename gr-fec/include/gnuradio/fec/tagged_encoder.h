/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_TAGGED_ENCODER_H
#define INCLUDED_FEC_TAGGED_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/tagged_stream_block.h>
#include <boost/shared_ptr.hpp>

namespace gr {
namespace fec {

/*!
 * \brief Creates the encoder block for use in GNU Radio
 * flowgraphs from a given FECAPI object derived from the
 * generic_encoder class.
 * \ingroup error_coding_blk
 *
 * \details
 *
 * Generally, we would use the fec.extended_encoder Python
 * implementation to instantiate this. The extended_encoder wraps
 * up a few more details, like taking care of puncturing as well
 * as the encoder itself.
 */
class FEC_API tagged_encoder : virtual public tagged_stream_block
{
public:
    typedef boost::shared_ptr<tagged_encoder> sptr;

    /*!
     * Build the FEC encoder block from an FECAPI encoder object.
     *
     * \param my_encoder An FECAPI encoder object child of the generic_encoder class.
     * \param input_item_size size of a block of data for the encoder.
     * \param output_item_size size of a block of data the encoder will produce.
     * \param lengthtagname Key name of the tagged stream frame size.
     * \param mtu The Maximum Transmission Unit (MTU) of the input
     *            frame that the block will be able to
     *            process. Specified in bytes and defaults to 1500.
     */
    static sptr make(generic_encoder::sptr my_encoder,
                     size_t input_item_size,
                     size_t output_item_size,
                     const std::string& lengthtagname = "packet_len",
                     int mtu = 1500);

    virtual int work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) = 0;
    virtual int calculate_output_stream_length(const gr_vector_int& ninput_items) = 0;
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_TAGGED_ENCODER_H */
