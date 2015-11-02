/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_H
#define INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Header/Payload demuxer.
     * \ingroup packet_operators_blk
     *
     * \details
     * This block is designed to demultiplex packets from a bursty transmission.
     * The typical application for this block is the case when you are receiving
     * packets with yet-to-determine length. This block will pass the header
     * section to other blocks for demodulation. Using the information from the
     * demodulated header, it will then output the payload. The beginning of the
     * header needs to be identified by a trigger signal (see below).
     *
     * \section hpd_theory_of_ops Theory of Operation
     *
     * Input 0 takes a continuous transmission of samples (items).
     * Input 1 is an optional input for the trigger signal (mark beginning of
     * packets). In this case, a non-zero value on input 1 identifies the beginning of a packet.
     * Otherwise, a tag with the key specified in \p trigger_tag_key is used as a
     * trigger (its value is irrelevant).
     *
     * Until a trigger signal is detected, all samples are dropped onto the floor.
     * Once a trigger is detected, a total of \p header_len items are copied to output 0.
     * The block then stalls until it receives a message on the message port
     * \p header_data. The message must be a PMT dictionary; all key/value pairs are
     * copied as tags to the first item of the payload (which is assumed to be the
     * first item after the header).
     * The value corresponding to the key specified in \p length_tag_key is read
     * and taken as the payload length. The payload, together with the header data
     * as tags, is then copied to output 1.
     *
     * \section hpd_item_sizes Symbols, Items and Item Sizes
     *
     * To generically and transparently handle different kinds of modulations,
     * including OFDM, this block distinguises between \b symbols and \b items.
     *
     * Items are what are consumed at the input. Anything that uses complex samples
     * will therefore use an itemsize of `sizeof(gr_complex)`. Symbols are a way of
     * grouping items. In OFDM, we usually don't care about individual samples, but
     * we do care about full OFDM symbols, so we set \p items_per_symbol to the
     * IFFT / FFT length of the OFDM modulator / demodulator.
     * For most single-carrier modulations, this value can be set to 1 (the default
     * value).
     * If specified, \p guard_interval items are discarded before every symbol.
     * This is useful for demuxing bursts of OFDM signals.
     *
     * On the output, we can deal with symbols directly by setting \p output_symbols
     * to true. In that case, the output item size is the <em>symbol size</em>.
     *
     * \b Example: OFDM with 48 sub-carriers, using a length-64 IFFT on the modulator,
     * and a cyclic-prefix length of 16 samples. In this case, the itemsize is
     * `sizeof(gr_complex)`, because we're receiving complex samples. One OFDM symbol
     * has 64 samples, hence \p items_per_symbol is set to 64, and \p guard_interval to
     * 16. The header length is specified in number of OFDM symbols. Because we want to
     * deal with full OFDM symbols, we set \p output_symbols to true.
     *
     * \section hpd_tag_handling Tag Handling
     *
     * Any tags on the input stream are copied to the corresponding output *if* they're
     * on an item that is propagated. Note that a tag on the header items is copied to the
     * header stream; that means the header-parsing block must handle these tags if they
     * should go on the payload.
     * A special case are tags on items that make up the guard interval. These are copied
     * to the first item of the following symbol.
     * If a tag is situated very close to the end of the payload, it might be unclear if
     * it belongs to this packet or the following. In this case, it is possible that the
     * tag might be propagated twice.
     *
     * Tags outside of packets are generally discarded. If this information is important,
     * there are two additional mechanisms to preserve the tags:
     * - Timing tags might be relevant to know \b when a packet was received. By
     *   specifying the name of a timestamp tag and the sample rate at this block, it
     *   keeps track of the time and will add the time to the first item of every packet.
     *   The name of the timestamp tag is usually 'rx_time' (see gr::uhd::usrp_source::make()).
     *   The time value must be specified in the UHD time format.
     * - Other tags are simply stored and updated. As an example, the user might want to know the
     *   rx frequency, which UHD stores in the rx_freq tag. In this case, add the tag name 'rx_freq'
     *   to the list of \p special_tags. This block will then always save the most current value of
     *   'rx_freq' and add it to the beginning of every packet.
     *
     */
    class DIGITAL_API header_payload_demux : virtual public block
    {
     public:
      typedef boost::shared_ptr<header_payload_demux> sptr;

      /*!
       * \param header_len Number of symbols per header
       * \param items_per_symbol Number of items per symbol
       * \param guard_interval Number of items between two consecutive symbols
       * \param length_tag_key Key of the frame length tag
       * \param trigger_tag_key Key of the trigger tag
       * \param output_symbols Output symbols (true) or items (false)?
       * \param itemsize Item size (bytes per item)
       * \param timing_tag_key The name of the tag with timing information, usually 'rx_time' or empty (this means timing info is discarded)
       * \param samp_rate Sampling rate at the input. Necessary to calculate the rx time of packets.
       * \param special_tags A vector of strings denoting tags which shall be preserved (see \ref hpd_tag_handling)
       */
      static sptr make(
          int header_len,
          int items_per_symbol=1,
          int guard_interval=0,
          const std::string &length_tag_key="frame_len",
          const std::string &trigger_tag_key="",
          bool output_symbols=false,
          size_t itemsize=sizeof(gr_complex),
          const std::string &timing_tag_key="",
          const double samp_rate=1.0,
          const std::vector<std::string> &special_tags=std::vector<std::string>()
      );
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_H */

