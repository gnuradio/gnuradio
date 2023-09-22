/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H
#define INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/ofdm_equalizer_base.h>

namespace gr {
namespace digital {

/*!
 * \brief Simple decision feedback equalizer for OFDM.
 * \ingroup ofdm_blk
 * \ingroup equalizers_blk
 *
 * \details
 * Equalizes an OFDM signal symbol by symbol using knowledge of the
 * complex modulations symbols.
 * For every symbol, the following steps are performed:
 * - On every sub-carrier, decode the modulation symbol
 * - Use the difference between the decoded symbol and the received symbol
 *   to update the channel state on this carrier
 * - Whenever a pilot symbol is found, it uses the known pilot symbol to
 *   update the channel state.
 *
 * This equalizer makes a lot of assumptions:
 * - The initial channel state is good enough to decode the first
 *   symbol without error (unless the first symbol only consists of pilot
 *   tones)
 * - The channel changes only very slowly, such that the channel state
 *   from one symbol is enough to decode the next
 * - SNR low enough that equalization will always suffice to correctly
 *   decode a symbol
 * If these assumptions are not met, the most common error is that the
 * channel state is estimated incorrectly during equalization; after that,
 * all subsequent symbols will be completely wrong.
 *
 * Note that, by default, the equalized symbols are *exact points* on the
 * constellation.
 * This means that soft information of the modulation symbols is lost after the
 * equalization, which is suboptimal for channel codes that use soft decision.
 * If this behaviour is not desired, set the `enable_soft_output` parameter to
 * true.
 *
 */
class DIGITAL_API ofdm_equalizer_simpledfe : public ofdm_equalizer_1d_pilots
{
public:
    typedef std::shared_ptr<ofdm_equalizer_simpledfe> sptr;

    ofdm_equalizer_simpledfe(int fft_len,
                             const gr::digital::constellation_sptr& constellation,
                             const std::vector<std::vector<int>>& occupied_carriers =
                                 std::vector<std::vector<int>>(),
                             const std::vector<std::vector<int>>& pilot_carriers =
                                 std::vector<std::vector<int>>(),
                             const std::vector<std::vector<gr_complex>>& pilot_symbols =
                                 std::vector<std::vector<gr_complex>>(),
                             int symbols_skipped = 0,
                             float alpha = 0.1,
                             bool input_is_shifted = true,
                             bool enable_soft_output = false);

    ~ofdm_equalizer_simpledfe() override;

    void equalize(gr_complex* frame,
                  int n_sym,
                  const std::vector<gr_complex>& initial_taps = std::vector<gr_complex>(),
                  const std::vector<tag_t>& tags = std::vector<tag_t>()) override;

    /*
     * \param fft_len FFT length
     * \param constellation The constellation object describing the modulation used
     *                      on the subcarriers (e.g. QPSK). This is used to decode
     *                      the individual symbols.
     * \param occupied_carriers List of occupied carriers, see ofdm_carrier_allocator
     *                          for a description.
     * \param pilot_carriers Position of pilot symbols, see ofdm_carrier_allocator
     *                          for a description.
     * \param pilot_symbols Value of pilot symbols, see ofdm_carrier_allocator
     *                          for a description.
     * \param alpha Averaging coefficient (in a nutshell, if \f$H_{i,k}\f$ is the channel
     *              state for carrier i and symbol k,
     *              \f$H_{i,k+1} =  \alpha H_{i,k} + (1 - \alpha) H_{i,k+1}\f$. Make this
     *              larger if there's more noise, but keep in mind that larger values
     *              of alpha mean slower response to channel changes).
     * \param symbols_skipped Starting position within occupied_carriers and
     * pilot_carriers. If the first symbol of the frame was removed (e.g. to decode the
     *                        header), set this make sure the pilot symbols are correctly
     *                        identified.
     * \param input_is_shifted Set this to false if the input signal is not shifted, i.e.
     *                         the first input items is on the DC carrier.
     *                         Note that a lot of the OFDM receiver blocks operate on
     * shifted signals!
     * \param enable_soft_output Output noisy equalized symbols instead of exact
     *                           constellation symbols.
     *                           This is useful for soft demodulation and decoding.
     */
    static sptr make(int fft_len,
                     const gr::digital::constellation_sptr& constellation,
                     const std::vector<std::vector<int>>& occupied_carriers =
                         std::vector<std::vector<int>>(),
                     const std::vector<std::vector<int>>& pilot_carriers =
                         std::vector<std::vector<int>>(),
                     const std::vector<std::vector<gr_complex>>& pilot_symbols =
                         std::vector<std::vector<gr_complex>>(),
                     int symbols_skipped = 0,
                     float alpha = 0.1,
                     bool input_is_shifted = true,
                     bool enable_soft_output = false);

private:
    gr::digital::constellation_sptr d_constellation;
    //! Averaging coefficient
    float d_alpha;
    //! Do not output exact constellation symbols
    bool d_enable_soft_output;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H */
