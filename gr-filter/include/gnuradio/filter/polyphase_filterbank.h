/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FILTER_POLYPHASE_FILTERBANK_H
#define INCLUDED_FILTER_POLYPHASE_FILTERBANK_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fir_filter.h>

namespace gr {
namespace filter {
namespace kernel {

/*!
 * \brief Polyphase filterbank parent class
 * \ingroup filter_blk
 * \ingroup pfb_blk
 *
 * \details
 * This block takes in complex inputs and channelizes it to
 * <EM>M</EM> channels of equal bandwidth. Each of the resulting
 * channels is decimated to the new rate that is the input
 * sampling rate <EM>fs</EM> divided by the number of channels,
 * <EM>M</EM>.
 *
 * The PFB channelizer code takes the taps generated above and
 * builds a set of filters. The set contains <EM>M</EM>
 * filters and each filter contains ceil(taps.size()/decim)
 * taps.  Each tap from the filter prototype is
 * sequentially inserted into the next filter. When all of the
 * input taps are used, the remaining filters in the filterbank
 * are filled out with 0's to make sure each filter has the same
 * number of taps.
 *
 * Each filter operates using the gr::filter::fir_filter_XXX
 * class of GNU Radio, which takes the input stream at
 * <EM>i</EM> and performs the inner product calculation to
 * <EM>i+(n-1)</EM> where <EM>n</EM> is the number of filter
 * taps. To efficiently handle this in the GNU Radio structure,
 * each filter input must come from its own input stream. So the
 * channelizer must be provided with <EM>M</EM> streams where
 * the input stream has been deinterleaved. This is most easily
 * done using the gr::blocks::stream_to_streams block.
 *
 * The output is then produced as a vector, where index
 * <EM>i</EM> in the vector is the next sample from the
 * <EM>i</EM>th channel. This is most easily handled by sending
 * the output to a gr::blocks::vector_to_streams block to handle
 * the conversion and passing <EM>M</EM> streams out.
 *
 * The input and output formatting is done using a hier_block2
 * called pfb_channelizer_ccf. This can take in a single stream
 * and outputs <EM>M</EM> streams based on the behavior
 * described above.
 *
 * The filter's taps should be based on the input sampling rate.
 *
 * For example, using the GNU Radio's firdes utility to building
 * filters, we build a low-pass filter with a sampling rate of
 * <EM>fs</EM>, a 3-dB bandwidth of <EM>BW</EM> and a transition
 * bandwidth of <EM>TB</EM>. We can also specify the out-of-band
 * attenuation to use, <EM>ATT</EM>, and the filter window
 * function (a Blackman-harris window in this case). The first
 * input is the gain of the filter, which we specify here as
 * unity.
 *
 *    <B><EM>self._taps = filter.firdes.low_pass_2(1, fs, BW, TB,
 *       attenuation_dB=ATT, window=fft.window.WIN_BLACKMAN_hARRIS)</EM></B>
 *
 * More on the theory of polyphase filterbanks can be found in
 * the following book:
 *
 *    <B><EM>f. harris, "Multirate Signal Processing for
 *       Communication Systems," Upper Saddle River, NJ:
 *       Prentice Hall, Inc. 2004.</EM></B>
 *
 */

class FILTER_API polyphase_filterbank
{
protected:
    unsigned int d_nfilts;
    std::vector<kernel::fir_filter_ccf> d_fir_filters;
    std::vector<kernel::fft_filter_ccf> d_fft_filters;
    std::vector<std::vector<float>> d_taps;
    unsigned int d_taps_per_filter;

    // The FFT to handle the output de-spinning of the channels.
    fft::fft_complex_rev d_fft;

public:
    /*!
     * Build the polyphase filterbank decimator.
     * \param nfilts (unsigned integer) Specifies the number of
     *               channels <EM>M</EM>
     * \param taps (vector/list of floats) The prototype filter to
     *             populate the filterbank.
     */
    polyphase_filterbank(unsigned int nfilts, const std::vector<float>& taps);

    virtual ~polyphase_filterbank() = default;

    /*!
     * Update the filterbank's filter taps from a prototype
     * filter.
     *
     * \param taps (vector/list of floats) The prototype filter to
     *             populate the filterbank.
     */
    virtual void set_taps(const std::vector<float>& taps);

    /*!
     * Print all of the filterbank taps to screen.
     */
    void print_taps();

    /*!
     * Return a vector<vector<>> of the filterbank taps
     */
    std::vector<std::vector<float>> taps() const;
};

} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_POLYPHASE_FILTERBANK_H */
