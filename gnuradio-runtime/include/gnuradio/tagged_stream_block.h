/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RUNTIME_TAGGED_STREAM_BLOCK_H
#define INCLUDED_GR_RUNTIME_TAGGED_STREAM_BLOCK_H

#include <gnuradio/api.h>
#include <gnuradio/block.h>

namespace gr {

  /*!
   * \brief Block that operates on PDUs in form of tagged streams
   * \ingroup base_blk
   *
   * Override work to provide the signal processing implementation.
   */
  class GR_RUNTIME_API tagged_stream_block : public block
  {
  private:
    pmt::pmt_t d_length_tag_key; //!< This is the key for the tag that stores the PDU length
    gr_vector_int d_n_input_items_reqd; //!< How many input items do I need to process the next PDU?

  protected:
    std::string d_length_tag_key_str;
    tagged_stream_block(void) {} // allows pure virtual interface sub-classes
    tagged_stream_block(const std::string &name,
                        gr::io_signature::sptr input_signature,
                        gr::io_signature::sptr output_signature,
                        const std::string &length_tag_key);

    /*!
     * \brief Parse all tags on the first sample of a PDU, return the
     *        number of items per input and prune the length tags.
     *
     * In most cases, you don't need to override this, unless the
     * number of items read is not directly coded in one single tag.
     *
     * Default behaviour:
     * - Go through all input ports
     * - On every input port, search for the tag with the key specified in \p length_tag_key
     * - Copy that value as an int to the corresponding position in \p n_input_items_reqd
     * - Remove the length tag.
     *
     * \param[in] tags All the tags found on the first item of every input port.
     * \param[out] n_input_items_reqd Number of items which will be read from every input
     */
    virtual void parse_length_tags(const std::vector<std::vector<tag_t> > &tags,
                                   gr_vector_int &n_input_items_reqd);

    /*!
     * \brief Calculate the number of output items.
     *
     * This is basically the inverse function to forecast(): Given a
     * number of input items, it returns the maximum number of output
     * items.
     *
     * You most likely need to override this function, unless your
     * block is a sync block or integer interpolator/decimator.
     */
    virtual int calculate_output_stream_length(const gr_vector_int &ninput_items);

    /*!
     * \brief Set the new length tags on the output stream
     *
     * Default behaviour: Set a tag with key \p length_tag_key and the
     * number of produced items on every output port.
     *
     * For anything else, override this.
     *
     * \param n_produced Length of the new PDU
     * \param n_ports Number of output ports
     */
    virtual void update_length_tags(int n_produced, int n_ports);

  public:
    /*! \brief Don't override this.
     */
    void /* final */ forecast (int noutput_items, gr_vector_int &ninput_items_required);

    bool check_topology(int ninputs, int /* noutputs */);

    /*!
     * - Reads the number of input items from the tags using parse_length_tags()
     * - Checks there's enough data on the input and output buffers
     * - If not, inform the scheduler and do nothing
     * - Calls work() with the exact number of items per PDU
     * - Updates the tags using update_length_tags()
     */
    int general_work(int noutput_items,
		     gr_vector_int &ninput_items,
		     gr_vector_const_void_star &input_items,
		     gr_vector_void_star &output_items);

    /*!
     * \brief Just like gr::block::general_work, but makes sure the input is valid
     *
     * The user must override work to define the signal processing
     * code. Check the documentation for general_work() to see what
     * happens here.
     *
     * Like gr::sync_block, this calls consume() for you (it consumes
     * ninput_items[i] items from the i-th port).
     *
     * A note on tag propagation: The PDU length tags are handled by
     * other functions, but all other tags are handled just as in any
     * other \p gr::block. So, most likely, you either set the tag
     * propagation policy to TPP_DONT and handle the tag propagation
     * manually, or you propagate tags through the scheduler and don't
     * do anything here.
     *
     * \param noutput_items The size of the writable output buffer
     * \param ninput_items The exact size of the items on every input for this particular PDU.
     *                     These will be consumed if a length tag key is provided!
     * \param input_items See gr::block
     * \param output_items See gr::block
     */
    virtual int work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items) = 0;
  };

}  /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_TAGGED_STREAM_BLOCK_H */

