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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include "correlate_and_sync_cc_impl.h"
#include <volk/volk.h>
#include <boost/format.hpp>
#include <boost/math/special_functions/round.hpp>
#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/filter/firdes.h>

namespace gr {
  namespace digital {

    correlate_and_sync_cc::sptr
    correlate_and_sync_cc::make(const std::vector<gr_complex> &symbols, float sps, float threshold)
    {
      return gnuradio::get_initial_sptr
        (new correlate_and_sync_cc_impl(symbols, sps, threshold));
    }

    correlate_and_sync_cc_impl::correlate_and_sync_cc_impl(const std::vector<gr_complex> &symbols, float sps, float threshold)
      : sync_block("correlate_and_sync_cc",
                   io_signature::make(1, 1, sizeof(gr_complex)),
                   io_signature::make(1, 2, sizeof(gr_complex))),
        d_src_id(pmt::intern(alias()))
    {
      d_sps = sps;

      d_symbols = symbols;
      for(size_t i=0; i<d_symbols.size(); i++) {
          d_symbols[i] = conj(d_symbols[i]);
      }
      std::reverse(d_symbols.begin(), d_symbols.end());

      // Compute a correlation threshold.
      // Compute the value of the discrete autocorrelation of the matched
      // filter with offset 0 (aka the autocorrelation peak).
      float corr = 0;
      for(size_t i=0; i < d_symbols.size(); i++)
        corr += abs(d_symbols[i]*conj(d_symbols[i]));
      d_thresh = threshold*corr*corr;

      // Correlation filter
      d_filter = new kernel::fft_filter_ccc(1, d_symbols);

      // Per comments in gr-filter/include/gnuradio/filter/fft_filter.h,
      // set the block output multiple to the FFT filter kernel's internal,
      // assumed "nsamples", to ensure the scheduler always passes a
      // proper number of samples.
      int nsamples;
      nsamples = d_filter->set_taps(d_symbols);
      set_output_multiple(nsamples);

      // It looks like the kernel::fft_filter_ccc stashes a tail between
      // calls, so that contains our filtering history (I think).  The
      // fft_filter_ccc block (which calls the kernel::fft_filter_ccc) sets
      // the history to 1 (0 history items), so let's follow its lead.
      //set_history(1);

      // We'll (ab)use the history for our own purposes of tagging back in time.
      // Keep a history of the length of the preamble to delay for tagging.
      set_history(d_symbols.size()+1);

      declare_sample_delay(1, 0);
      declare_sample_delay(0, d_symbols_size());

      // Setting the alignment multiple for volk causes problems with the
      // expected behavior of setting the output multiple for the FFT filter.
      // Don't set the alignment multiple.
      //const int alignment_multiple =
      //  volk_get_alignment() / sizeof(gr_complex);
      //set_alignment(std::max(1,alignment_multiple));

      // In order to easily support the optional second output,
      // don't deal with an unbounded max number of output items
      set_max_noutput_items(24*1024);
      d_corr = (gr_complex *) malloc(sizeof(gr_complex)*24*1024);
      d_corr_mag = (float *) malloc(sizeof(float)*24*1024);
    }

    correlate_and_sync_cc_impl::~correlate_and_sync_cc_impl()
    {
      delete d_filter;
      free(d_corr);
      free(d_corr_mag);
    }

    std::vector<gr_complex>
    correlate_and_sync_cc_impl::symbols() const
    {
      return d_symbols;
    }

    void
    correlate_and_sync_cc_impl::set_symbols(const std::vector<gr_complex> &symbols)
    {
      gr::thread::scoped_lock lock(d_setlock);

      d_symbols = symbols;

      // Per comments in gr-filter/include/gnuradio/filter/fft_filter.h,
      // set the block output multiple to the FFT filter kernel's internal,
      // assumed "nsamples", to ensure the scheduler always passes a
      // proper number of samples.
      int nsamples;
      nsamples = d_filter->set_taps(d_symbols);
      set_output_multiple(nsamples);

      // It looks like the kernel::fft_filter_ccc stashes a tail between
      // calls, so that contains our filtering history (I think).  The
      // fft_filter_ccc block (which calls the kernel::fft_filter_ccc) sets
      // the history to 1 (0 history items), so let's follow its lead.
      //set_history(1);

      // We'll (ab)use the history for our own purposes of tagging back in time.
      // Keep a history of the length of the preamble to delay for tagging.
      set_history(d_symbols.size()+1);

      declare_sample_delay(1, 0);
      declare_sample_delay(0, d_symbols_size());
    }

    int
    correlate_and_sync_cc_impl::work(int noutput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock lock(d_setlock);

      const gr_complex *in = (gr_complex *)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      gr_complex *corr;
      if (output_items.size() > 1)
          corr = (gr_complex *) output_items[1];
      else
          corr = d_corr;

      // Our correlation filter length
      unsigned int hist_len = history() - 1;

      // Delay the output by our correlation filter length so
      // we can tag backwards in time
      memcpy(out, &in[0], sizeof(gr_complex)*noutput_items);

      // Calculate the correlation of the non-delayed input with the known
      // symbols.
      d_filter->filter(noutput_items, &in[hist_len], corr);

      // Find the magnitude squared of the correlation
      volk_32fc_magnitude_squared_32f(&d_corr_mag[0], corr, noutput_items);

      int isps = (int) (d_sps + 0.5f);
      int i = 0;
      while(i < noutput_items) {
        // Look for the correlator output to cross the threshold
        if (d_corr_mag[i] <= d_thresh) {
          i++;
          continue;
        }
        // Go to (just past) the current correlator output peak
        while ((i < (noutput_items-1)) and
               (d_corr_mag[i] < d_corr_mag[i+1]))
          i++;

        // Delaying the primary signal output by the matched filter
        // length using history(), means that the the peak output of the
        // matched filter aligns with the start of the desired preamble in
        // the primary signal output.
        // This corr_start tag is not offset to another sample, so that
        // downstream data-aided blocks (like adaptive equalizers) know
        // exactly where the start of the correlated symbols are.
        add_item_tag(0, nitems_written(0) + i, pmt::intern("corr_start"),
                     pmt::from_double(d_corr_mag[i]), d_src_id);

        // Peak detector using a "center of mass" approach
        // center holds the +/- fraction of a sample index
        // from the found peak index to the estimated actual peak index.
        double center = 0.0;
        if (i > 0 and i < (noutput_items - 1)) {
          double nom = 0, den = 0;
          for(int s = 0; s < 3; s++) {
            nom += (s+1)*d_corr_mag[i+s-1];
            den += d_corr_mag[i+s-1];
          }
          center = nom / den - 2.0;
        }

        // In the old implementation of this block, a single padding symbol
        // in the matched filter caused the matched filter to have
        // ~d_sps/2.0 samples added at the begining and end of the matched
        // filter.  The old implementation implemented an index into the center
        // of the first correlated symbol by adding d_sps from the beginning
        // of the correlated sequence.
        //
        // Since we don't know if we have any padding at the start of the
        // user provided symbols, assume we don't and index in by d_sps/2.0
        // to get to the center of the first correlated symbol.
        int index = i + (int) (d_sps/2.0 + 0.5f) + 1;

        // Calculate the phase offset of the incoming signal; always
        // adjust it based on the proper rotation of the expected
        // known word; rotate by pi is the real part is < 0 since
        // the atan doesn't understand the ambiguity.
        float phase = fast_atan2f(corr[index].imag(), corr[index].real());
        if(corr[index].real() < 0.0)
          phase += M_PI;

        add_item_tag(0, nitems_written(0) + index, pmt::intern("phase_est"),
                     pmt::from_double(phase), d_src_id);
        add_item_tag(0, nitems_written(0) + index, pmt::intern("time_est"),
                     pmt::from_double(center), d_src_id);
        // N.B. the appropriate d_corr_mag[] index is "i", not "index".
        add_item_tag(0, nitems_written(0) + index, pmt::intern("corr_est"),
                     pmt::from_double(d_corr_mag[i]), d_src_id);

        if (output_items.size() > 1) {
          // N.B. these tags are not offset by half a symbol
          add_item_tag(1, nitems_written(0) + i, pmt::intern("phase_est"),
                       pmt::from_double(phase), d_src_id);
          add_item_tag(1, nitems_written(0) + i, pmt::intern("time_est"),
                       pmt::from_double(center), d_src_id);
          add_item_tag(1, nitems_written(0) + i, pmt::intern("corr_est"),
                       pmt::from_double(d_corr_mag[i]), d_src_id);
        }

        // Skip ahead to the next potential symbol peak
        // (for non-offset/interleaved symbols)
        i += isps;
      }
      if (output_items.size() > 1)
        add_item_tag(1, nitems_written(0) + noutput_items - 1,
                     pmt::intern("cas_eow"), pmt::from_uint64(noutput_items),
                     d_src_id);

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */

