/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_TAGGED_DECODER_H
#define INCLUDED_FEC_TAGGED_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/tagged_stream_block.h>
#include <boost/shared_array.hpp>
#include <memory>

namespace gr {
namespace fec {

/*!
 * \brief General FEC decoding block that takes in a decoder
 * variable object (derived from gr::fec::general_decoder) for use
 * in a flowgraph.
 *
 * \ingroup error_coding_blk
 *
 * \details
 * This block uses a decoder variable object (derived from
 * gr::fec::generic_decoder) to decode data within a
 * flowgraph. This block interacts with the general FECAPI
 * architecture to handle all passing all input and output data in
 * a flowgraph. The decoder variable takes care of understanding
 * the requirements, data types and sizes, and boundary conditions
 * of the specific FEC decoding algorithm.
 *
 * Generally, this block is used within the fec.extended_decoder
 * Python block to handle some input/output formatting issues. In
 * the FECAPI, the decoder variable sets properties like the input
 * and output types and sizes and whether the output is packed or
 * unpacked bytes. The fec.extended_decoder uses this information
 * to set up an gr::hier_block2 structure to make sure the I/O to
 * the variable is handled consistently, such as to make sure all
 * inputs are floats with one soft symbol per item and the outputs
 * are unpacked bytes with the bit in the LSB.
 *
 * See gr::fec::generic_decoder for detail on what information an
 * FECAPI variable object can set if using this block directly and
 * not as part of the fec.extended_decoder.
 */
class FEC_API tagged_decoder : virtual public tagged_stream_block
{
public:
    typedef std::shared_ptr<tagged_decoder> sptr;
    typedef boost::shared_array<unsigned char> buf_sptr;

    /*!
     * Create the FEC decoder block by taking in the FECAPI decoder
     * object as well as input and output sizes.
     *
     * \param my_decoder An FECAPI decoder object (See gr::fec::generic_decoder).
     * \param input_item_size The size of the input items (often the my_decoder object can
     * tell us this). \param output_item_size The size of the output items (often the
     * my_decoder object can tell us this). \param lengthtagname Key name of the tagged
     * stream frame size. \param mtu The Maximum Transmission Unit (MTU) of the output
     *            frame that the block will be able to
     *            process. Specified in bytes and defaults to 1500.
     */
    static sptr make(generic_decoder::sptr my_decoder,
                     size_t input_item_size,
                     size_t output_item_size,
                     const std::string& lengthtagname = "packet_len",
                     int mtu = 1500);

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override = 0;
    int calculate_output_stream_length(const gr_vector_int& ninput_items) override = 0;
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_TAGGED_DECODER_H */
