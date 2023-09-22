/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H
#define INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/ofdm_equalizer_base.h>

namespace gr {
namespace digital {

/*!
 * \brief Very simple static equalizer for OFDM.
 * \ingroup ofdm_blk
 * \ingroup equalizers_blk
 *
 * \details
 * This is an extremely simple equalizer. It will only work for
 * high-SNR, very, very slowly changing channels.
 *
 * It simply divides the signal with the currently known channel
 * state. Whenever a pilot symbol comes around, it updates the
 * channel state on that particular carrier by dividing the
 * received symbol with the known pilot symbol.
 */
class DIGITAL_API ofdm_equalizer_static : public ofdm_equalizer_1d_pilots
{
public:
    typedef std::shared_ptr<ofdm_equalizer_static> sptr;

    ofdm_equalizer_static(int fft_len,
                          const std::vector<std::vector<int>>& occupied_carriers =
                              std::vector<std::vector<int>>(),
                          const std::vector<std::vector<int>>& pilot_carriers =
                              std::vector<std::vector<int>>(),
                          const std::vector<std::vector<gr_complex>>& pilot_symbols =
                              std::vector<std::vector<gr_complex>>(),
                          int symbols_skipped = 0,
                          bool input_is_shifted = true);
    ~ofdm_equalizer_static() override;

    /*! \brief Divide the input signal with the current channel state.
     *
     * Does the following (and nothing else):
     * - Divide every OFDM symbol with the current channel state
     * - If a pilot symbol is found, re-set the channel state by dividing the received
     *   symbol with the known pilot symbol
     */
    void equalize(gr_complex* frame,
                  int n_sym,
                  const std::vector<gr_complex>& initial_taps = std::vector<gr_complex>(),
                  const std::vector<tag_t>& tags = std::vector<tag_t>()) override;

    /*
     * \param fft_len FFT length
     * \param occupied_carriers List of occupied carriers, see ofdm_carrier_allocator
     *                          for a description.
     * \param pilot_carriers Position of pilot symbols, see ofdm_carrier_allocator
     *                          for a description.
     * \param pilot_symbols Value of pilot symbols, see ofdm_carrier_allocator
     *                          for a description.
     * \param symbols_skipped Starting position within occupied_carriers and
     * pilot_carriers. If the first symbol of the frame was removed (e.g. to decode the
     *                        header), set this make sure the pilot symbols are correctly
     *                        identified.
     * \param input_is_shifted Set this to false if the input signal is not shifted, i.e.
     *                         the first input items is on the DC carrier.
     *                         Note that a lot of the OFDM receiver blocks operate on
     * shifted signals!
     */
    static sptr make(int fft_len,
                     const std::vector<std::vector<int>>& occupied_carriers =
                         std::vector<std::vector<int>>(),
                     const std::vector<std::vector<int>>& pilot_carriers =
                         std::vector<std::vector<int>>(),
                     const std::vector<std::vector<gr_complex>>& pilot_symbols =
                         std::vector<std::vector<gr_complex>>(),
                     int symbols_skipped = 0,
                     bool input_is_shifted = true);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H */
