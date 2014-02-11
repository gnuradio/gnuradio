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


#ifndef INCLUDED_PFB_ARB_RESAMPLER_H
#define	INCLUDED_PFB_ARB_RESAMPLER_H

#include <gnuradio/filter/fir_filter.h>

namespace gr {
  namespace filter {
    namespace kernel {

      /*!
       * \brief Polyphase filterbank arbitrary resampler with
       *        gr_complex input, gr_complex output and float taps
       * \ingroup resamplers_blk
       *
       * \details
       * This  takes in a signal stream and performs arbitrary
       * resampling. The resampling rate can be any real number
       * <EM>r</EM>. The resampling is done by constructing <EM>N</EM>
       * filters where <EM>N</EM> is the interpolation rate.  We then
       * calculate <EM>D</EM> where <EM>D = floor(N/r)</EM>.
       *
       * Using <EM>N</EM> and <EM>D</EM>, we can perform rational
       * resampling where <EM>N/D</EM> is a rational number close to
       * the input rate <EM>r</EM> where we have <EM>N</EM> filters
       * and we cycle through them as a polyphase filterbank with a
       * stride of <EM>D</EM> so that <EM>i+1 = (i + D) % N</EM>.
       *
       * To get the arbitrary rate, we want to interpolate between two
       * points. For each value out, we take an output from the
       * current filter, <EM>i</EM>, and the next filter <EM>i+1</EM>
       * and then linearly interpolate between the two based on the
       * real resampling rate we want.
       *
       * The linear interpolation only provides us with an
       * approximation to the real sampling rate specified. The error
       * is a quantization error between the two filters we used as
       * our interpolation points.  To this end, the number of
       * filters, <EM>N</EM>, used determines the quantization error;
       * the larger <EM>N</EM>, the smaller the noise. You can design
       * for a specified noise floor by setting the filter size
       * (parameters <EM>filter_size</EM>). The size defaults to 32
       * filters, which is about as good as most implementations need.
       *
       * The trick with designing this filter is in how to specify the
       * taps of the prototype filter. Like the PFB interpolator, the
       * taps are specified using the interpolated filter rate. In
       * this case, that rate is the input sample rate multiplied by
       * the number of filters in the filterbank, which is also the
       * interpolation rate. All other values should be relative to
       * this rate.
       *
       * For example, for a 32-filter arbitrary resampler and using
       * the GNU Radio's firdes utility to build the filter, we build
       * a low-pass filter with a sampling rate of <EM>fs</EM>, a 3-dB
       * bandwidth of <EM>BW</EM> and a transition bandwidth of
       * <EM>TB</EM>. We can also specify the out-of-band attenuation
       * to use, <EM>ATT</EM>, and the filter window function (a
       * Blackman-harris window in this case). The first input is the
       * gain of the filter, which we specify here as the
       * interpolation rate (<EM>32</EM>).
       *
       *   <B><EM>self._taps = filter.firdes.low_pass_2(32, 32*fs, BW, TB,
       *      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
       *
       * The theory behind this block can be found in Chapter 7.5 of
       * the following book.
       *
       *   <B><EM>f. harris, "Multirate Signal Processing for Communication
       *      Systems", Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
       */
      class FILTER_API pfb_arb_resampler_ccf
      {
      private:
        std::vector<fir_filter_ccf*> d_filters;
        std::vector<fir_filter_ccf*> d_diff_filters;
        std::vector< std::vector<float> > d_taps;
        std::vector< std::vector<float> > d_dtaps;
        unsigned int d_int_rate;          // the number of filters (interpolation rate)
        unsigned int d_dec_rate;          // the stride through the filters (decimation rate)
        float        d_flt_rate;          // residual rate for the linear interpolation
        float        d_acc;               // accumulator; holds fractional part of sample
        unsigned int d_last_filter;       // stores filter for re-entry
        unsigned int d_taps_per_filter;   // num taps for each arm of the filterbank
        int d_delay;                      // filter's group delay
        float d_est_phase_change;         // est. of phase change of a sine wave through filt.

        /*!
         * Takes in the taps and convolves them with [-1,0,1], which
         * creates a differential set of taps that are used in the
         * difference filterbank.
         * \param newtaps (vector of floats) The prototype filter.
         * \param difftaps (vector of floats) (out) The differential filter taps.
         */
        void create_diff_taps(const std::vector<float> &newtaps,
                              std::vector<float> &difftaps);

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param newtaps    (vector of floats) The prototype filter to populate the filterbank.
         *                   The taps should be generated at the interpolated sampling rate.
         * \param ourtaps    (vector of floats) Reference to our internal member of holding the taps.
         * \param ourfilter  (vector of filters) Reference to our internal filter to set the taps for.
         */
        void create_taps(const std::vector<float> &newtaps,
                         std::vector< std::vector<float> > &ourtaps,
                         std::vector<kernel::fir_filter_ccf*> &ourfilter);

      public:
        /*!
         * Creates a kernel to perform arbitrary resampling on a set of samples.
         * \param rate  (float) Specifies the resampling rate to use
         * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps       *              should be generated at the filter_size sampling rate.
         * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
         *                    related to quantization noise introduced during the resampling.
         *                    Defaults to 32 filters.
         */
        pfb_arb_resampler_ccf(float rate,
                              const std::vector<float> &taps,
                              unsigned int filter_size);

        ~pfb_arb_resampler_ccf();

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param taps (vector/list of floats) The prototype filter to populate the filterbank.
         */
        void set_taps(const std::vector<float> &taps);

        /*!
         * Return a vector<vector<>> of the filterbank taps
         */
        std::vector<std::vector<float> > taps() const;

        /*!
         * Print all of the filterbank taps to screen.
         */
        void print_taps();

        /*!
         * Sets the resampling rate of the block.
         */
        void set_rate(float rate);

        /*!
         * Sets the current phase offset in radians (0 to 2pi).
         */
        void set_phase(float ph);

        /*!
         * Gets the current phase of the resampler in radians (2 to 2pi).
         */
        float phase() const;

        /*!
         * Gets the number of taps per filter.
         */
        unsigned int taps_per_filter() const;

        unsigned int interpolation_rate() const { return d_int_rate; }
        unsigned int decimation_rate() const { return d_dec_rate; }
        float fractional_rate() const { return d_flt_rate; }

        /*!
         * Get the group delay of the filter.
         */
        int group_delay() const { return d_delay; }

        /*!
         * Calculates the phase offset expected by a sine wave of
         * frequency \p freq and sampling rate \p fs (assuming input
         * sine wave has 0 degree phase).
         */
        float phase_offset(float freq, float fs);

        /*!
         * Performs the filter operation that resamples the signal.
         *
         * This block takes in a stream of samples and outputs a
         * resampled and filtered stream. This block should be called
         * such that the output has \p rate * \p n_to_read amount of
         * space available in the \p output buffer.
         *
         * \param input An input vector of samples to be resampled
         * \param output The output samples at the new sample rate.
         * \param n_to_read Number of samples to read from \p input.
         * \param n_read (out) Number of samples actually read from \p input.
         * \return Number of samples put into \p output.
         */
        int filter(gr_complex *input, gr_complex *output,
                   int n_to_read, int &n_read);
      };


      /**************************************************************/


      class FILTER_API pfb_arb_resampler_ccc
      {
      private:
        std::vector<fir_filter_ccc*> d_filters;
        std::vector<fir_filter_ccc*> d_diff_filters;
        std::vector< std::vector<gr_complex> > d_taps;
        std::vector< std::vector<gr_complex> > d_dtaps;
        unsigned int d_int_rate;          // the number of filters (interpolation rate)
        unsigned int d_dec_rate;          // the stride through the filters (decimation rate)
        float        d_flt_rate;          // residual rate for the linear interpolation
        float        d_acc;               // accumulator; holds fractional part of sample
        unsigned int d_last_filter;       // stores filter for re-entry
        unsigned int d_taps_per_filter;   // num taps for each arm of the filterbank
        int d_delay;                      // filter's group delay
        float d_est_phase_change;         // est. of phase change of a sine wave through filt.

        /*!
         * Takes in the taps and convolves them with [-1,0,1], which
         * creates a differential set of taps that are used in the
         * difference filterbank.
         * \param newtaps (vector of complex) The prototype filter.
         * \param difftaps (vector of complex) (out) The differential filter taps.
         */
        void create_diff_taps(const std::vector<gr_complex> &newtaps,
                              std::vector<gr_complex> &difftaps);

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param newtaps    (vector of complex) The prototype filter to populate the filterbank.
         *                   The taps should be generated at the interpolated sampling rate.
         * \param ourtaps    (vector of complex) Reference to our internal member of holding the taps.
         * \param ourfilter  (vector of ccc filters) Reference to our internal filter to set the taps for.
         */
        void create_taps(const std::vector<gr_complex> &newtaps,
                         std::vector< std::vector<gr_complex> > &ourtaps,
                         std::vector<kernel::fir_filter_ccc*> &ourfilter);

      public:
        /*!
         * Creates a kernel to perform arbitrary resampling on a set of samples.
         * \param rate  (float) Specifies the resampling rate to use
         * \param taps  (vector/list of complex) The prototype filter to populate the filterbank. The taps       *              should be generated at the filter_size sampling rate.
         * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
         *                    related to quantization noise introduced during the resampling.
         *                    Defaults to 32 filters.
         */
        pfb_arb_resampler_ccc(float rate,
                              const std::vector<gr_complex> &taps,
                              unsigned int filter_size);

        ~pfb_arb_resampler_ccc();

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param taps (vector/list of complex) The prototype filter to populate the filterbank.
         */
        void set_taps(const std::vector<gr_complex> &taps);

        /*!
         * Return a vector<vector<>> of the filterbank taps
         */
        std::vector<std::vector<gr_complex> > taps() const;

        /*!
         * Print all of the filterbank taps to screen.
         */
        void print_taps();

        /*!
         * Sets the resampling rate of the block.
         */
        void set_rate(float rate);

        /*!
         * Sets the current phase offset in radians (0 to 2pi).
         */
        void set_phase(float ph);

        /*!
         * Gets the current phase of the resampler in radians (2 to 2pi).
         */
        float phase() const;

        /*!
         * Gets the number of taps per filter.
         */
        unsigned int taps_per_filter() const;

        unsigned int interpolation_rate() const { return d_int_rate; }
        unsigned int decimation_rate() const { return d_dec_rate; }
        float fractional_rate() const { return d_flt_rate; }

        /*!
         * Get the group delay of the filter.
         */
        int group_delay() const { return d_delay; }

        /*!
         * Calculates the phase offset expected by a sine wave of
         * frequency \p freq and sampling rate \p fs (assuming input
         * sine wave has 0 degree phase).
         */
        float phase_offset(float freq, float fs);

        /*!
         * Performs the filter operation that resamples the signal.
         *
         * This block takes in a stream of samples and outputs a
         * resampled and filtered stream. This block should be called
         * such that the output has \p rate * \p n_to_read amount of
         * space available in the \p output buffer.
         *
         * \param input An input vector of samples to be resampled
         * \param output The output samples at the new sample rate.
         * \param n_to_read Number of samples to read from \p input.
         * \param n_read (out) Number of samples actually read from \p input.
         * \return Number of samples put into \p output.
         */
        int filter(gr_complex *input, gr_complex *output,
                   int n_to_read, int &n_read);
      };


      /**************************************************************/


      /*!
       * \brief Polyphase filterbank arbitrary resampler with
       *        float input, float output and float taps
       * \ingroup resamplers_blk
       *
       * \details
       * This  takes in a signal stream and performs arbitrary
       * resampling. The resampling rate can be any real number
       * <EM>r</EM>. The resampling is done by constructing <EM>N</EM>
       * filters where <EM>N</EM> is the interpolation rate.  We then
       * calculate <EM>D</EM> where <EM>D = floor(N/r)</EM>.
       *
       * Using <EM>N</EM> and <EM>D</EM>, we can perform rational
       * resampling where <EM>N/D</EM> is a rational number close to
       * the input rate <EM>r</EM> where we have <EM>N</EM> filters
       * and we cycle through them as a polyphase filterbank with a
       * stride of <EM>D</EM> so that <EM>i+1 = (i + D) % N</EM>.
       *
       * To get the arbitrary rate, we want to interpolate between two
       * points. For each value out, we take an output from the
       * current filter, <EM>i</EM>, and the next filter <EM>i+1</EM>
       * and then linearly interpolate between the two based on the
       * real resampling rate we want.
       *
       * The linear interpolation only provides us with an
       * approximation to the real sampling rate specified. The error
       * is a quantization error between the two filters we used as
       * our interpolation points.  To this end, the number of
       * filters, <EM>N</EM>, used determines the quantization error;
       * the larger <EM>N</EM>, the smaller the noise. You can design
       * for a specified noise floor by setting the filter size
       * (parameters <EM>filter_size</EM>). The size defaults to 32
       * filters, which is about as good as most implementations need.
       *
       * The trick with designing this filter is in how to specify the
       * taps of the prototype filter. Like the PFB interpolator, the
       * taps are specified using the interpolated filter rate. In
       * this case, that rate is the input sample rate multiplied by
       * the number of filters in the filterbank, which is also the
       * interpolation rate. All other values should be relative to
       * this rate.
       *
       * For example, for a 32-filter arbitrary resampler and using
       * the GNU Radio's firdes utility to build the filter, we build
       * a low-pass filter with a sampling rate of <EM>fs</EM>, a 3-dB
       * bandwidth of <EM>BW</EM> and a transition bandwidth of
       * <EM>TB</EM>. We can also specify the out-of-band attenuation
       * to use, <EM>ATT</EM>, and the filter window function (a
       * Blackman-harris window in this case). The first input is the
       * gain of the filter, which we specify here as the
       * interpolation rate (<EM>32</EM>).
       *
       *   <B><EM>self._taps = filter.firdes.low_pass_2(32, 32*fs, BW, TB,
       *      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
       *
       * The theory behind this block can be found in Chapter 7.5 of
       * the following book.
       *
       *   <B><EM>f. harris, "Multirate Signal Processing for Communication
       *      Systems", Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
       */
      class FILTER_API pfb_arb_resampler_fff
      {
      private:
        std::vector<fir_filter_fff*> d_filters;
        std::vector<fir_filter_fff*> d_diff_filters;
        std::vector< std::vector<float> > d_taps;
        std::vector< std::vector<float> > d_dtaps;
        unsigned int d_int_rate;          // the number of filters (interpolation rate)
        unsigned int d_dec_rate;          // the stride through the filters (decimation rate)
        float        d_flt_rate;          // residual rate for the linear interpolation
        float        d_acc;               // accumulator; holds fractional part of sample
        unsigned int d_last_filter;       // stores filter for re-entry
        unsigned int d_taps_per_filter;   // num taps for each arm of the filterbank
        int d_delay;                      // filter's group delay
        float d_est_phase_change;         // est. of phase change of a sine wave through filt.

        /*!
         * Takes in the taps and convolves them with [-1,0,1], which
         * creates a differential set of taps that are used in the
         * difference filterbank.
         * \param newtaps (vector of floats) The prototype filter.
         * \param difftaps (vector of floats) (out) The differential filter taps.
         */
        void create_diff_taps(const std::vector<float> &newtaps,
                              std::vector<float> &difftaps);

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param newtaps    (vector of floats) The prototype filter to populate the filterbank.
         *                   The taps should be generated at the interpolated sampling rate.
         * \param ourtaps    (vector of floats) Reference to our internal member of holding the taps.
         * \param ourfilter  (vector of filters) Reference to our internal filter to set the taps for.
         */
        void create_taps(const std::vector<float> &newtaps,
                         std::vector< std::vector<float> > &ourtaps,
                         std::vector<kernel::fir_filter_fff*> &ourfilter);

      public:
        /*!
         * Creates a kernel to perform arbitrary resampling on a set of samples.
         * \param rate  (float) Specifies the resampling rate to use
         * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps       *              should be generated at the filter_size sampling rate.
         * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
         *                    related to quantization noise introduced during the resampling.
         *                    Defaults to 32 filters.
         */
        pfb_arb_resampler_fff(float rate,
                              const std::vector<float> &taps,
                              unsigned int filter_size);

        ~pfb_arb_resampler_fff();

        /*!
         * Resets the filterbank's filter taps with the new prototype filter
         * \param taps (vector/list of floats) The prototype filter to populate the filterbank.
         */
        void set_taps(const std::vector<float> &taps);

        /*!
         * Return a vector<vector<>> of the filterbank taps
         */
        std::vector<std::vector<float> > taps() const;

        /*!
         * Print all of the filterbank taps to screen.
         */
        void print_taps();

        /*!
         * Sets the resampling rate of the block.
         */
        void set_rate(float rate);

        /*!
         * Sets the current phase offset in radians (0 to 2pi).
         */
        void set_phase(float ph);

        /*!
         * Gets the current phase of the resampler in radians (2 to 2pi).
         */
        float phase() const;

        /*!
         * Gets the number of taps per filter.
         */
        unsigned int taps_per_filter() const;

        unsigned int interpolation_rate() const { return d_int_rate; }
        unsigned int decimation_rate() const { return d_dec_rate; }
        float fractional_rate() const { return d_flt_rate; }

        /*!
         * Get the group delay of the filter.
         */
        int group_delay() const { return d_delay; }

        /*!
         * Calculates the phase offset expected by a sine wave of
         * frequency \p freq and sampling rate \p fs (assuming input
         * sine wave has 0 degree phase).
         */
        float phase_offset(float freq, float fs);

        /*!
         * Performs the filter operation that resamples the signal.
         *
         * This block takes in a stream of samples and outputs a
         * resampled and filtered stream. This block should be called
         * such that the output has \p rate * \p n_to_read amount of
         * space available in the \p output buffer.
         *
         * \param input An input vector of samples to be resampled
         * \param output The output samples at the new sample rate.
         * \param n_to_read Number of samples to read from \p input.
         * \param n_read (out) Number of samples actually read from \p input.
         * \return Number of samples put into \p output.
         */
        int filter(float *input, float *output,
                   int n_to_read, int &n_read);
      };

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_H */
