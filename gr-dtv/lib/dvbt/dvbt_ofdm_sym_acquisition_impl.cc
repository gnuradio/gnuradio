/* -*- c++ -*- */
/*
 * Copyright 2015,2016,2018 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvbt_ofdm_sym_acquisition_impl.h"
#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>
#include <complex>
#include <limits>

namespace gr {
namespace dtv {

namespace {
float calculate_rho(float snr)
{
    snr = pow(10, snr / 10.0);
    return snr / (snr + 1.0);
}
} // namespace

int dvbt_ofdm_sym_acquisition_impl::peak_detect_init(float threshold_factor_rise,
                                                     float alpha)
{
    d_avg_alpha = alpha;
    d_threshold_factor_rise = threshold_factor_rise;
    d_avg_max = std::numeric_limits<float>::min();
    d_avg_min = std::numeric_limits<float>::max();

    return (0);
}

int dvbt_ofdm_sym_acquisition_impl::peak_detect_process(const float* datain,
                                                        const int datain_length,
                                                        int* peak_pos,
                                                        int* peak_max)
{
    uint16_t peak_index = 0;
    int peak_pos_length = 0;

    volk_32f_index_max_16u(&peak_index, &datain[0], datain_length);

    peak_pos_length = 1;
    if (datain_length >= d_fft_length) {
        float min = datain[(peak_index + d_fft_length / 2) % d_fft_length];
        if (d_avg_min == std::numeric_limits<float>::max()) {
            d_avg_min = min;
        } else {
            d_avg_min = d_avg_alpha * min + (1 - d_avg_alpha) * d_avg_min;
        }
    }

    if (d_avg_max == std::numeric_limits<float>::min()) {
        // Initialize d_avg_max with the first value.
        d_avg_max = datain[peak_index];
    } else if (datain[peak_index] >
               d_avg_max - d_threshold_factor_rise * (d_avg_max - d_avg_min)) {
        d_avg_max = d_avg_alpha * datain[peak_index] + (1 - d_avg_alpha) * d_avg_max;
    } else {
        peak_pos_length = 0;
    }

    // We now check whether the peak is in the border of the search interval. This would
    // mean that the search interval is not correct, and it should be re-set. This happens
    // for instance when the hardware dropped some samples. Our definition of "border of
    // the search interval" depends if the search interval is "big" or not.
    if (datain_length < d_fft_length) {
        if ((peak_index == 0) || (peak_index == (unsigned int)datain_length - 1)) {
            peak_pos_length = 0;
        }
    } else {
        if ((peak_index < 5) || (peak_index > (unsigned int)datain_length - 5)) {
            peak_pos_length = 0;
        }
    }

    peak_pos[0] = peak_index;
    *peak_max = 0;
    return (peak_pos_length);
}

int dvbt_ofdm_sym_acquisition_impl::ml_sync(const gr_complex* in,
                                            int lookup_start,
                                            int lookup_stop,
                                            int* cp_pos,
                                            gr_complex* derot,
                                            int* to_consume,
                                            int* to_out)
{
    assert(lookup_start >= lookup_stop);
    assert(lookup_stop >= (d_cp_length + d_fft_length - 1));

    int low, size;

    // Array to store peak positions
    __GR_VLA(int, peak_pos, d_fft_length);
    __GR_VLA(float, d_phi, d_fft_length);

    // Calculate norm
    low = lookup_stop - (d_cp_length + d_fft_length - 1);
    size = lookup_start - (lookup_stop - (d_cp_length + d_fft_length - 1)) + 1;

    volk_32fc_magnitude_squared_32f(&d_norm[low], &in[low], size);

    // Calculate gamma on each point
    // TODO check these boundaries!!!!!!!
    low = lookup_stop - (d_cp_length - 1);
    size = lookup_start - low + 1;

    volk_32fc_x2_multiply_conjugate_32fc(
        &d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);

    // Calculate time delay and frequency correction
    // This looks like spaghetti code but it is fast
    for (int i = lookup_start - 1; i >= lookup_stop; i--) {
        int k = i - lookup_stop;

        d_phi[k] = 0.0;
        d_gamma[k] = 0.0;

        // Moving sum for calculating gamma and phi
        for (int j = 0; j < d_cp_length; j++) {
            // Calculate gamma and store it
            d_gamma[k] += d_corr[i - j - d_fft_length];
            // Calculate phi and store it
            d_phi[k] += d_norm[i - j] + d_norm[i - j - d_fft_length];
        }
    }

    // Init lambda with gamma
    low = 0;
    size = lookup_start - lookup_stop;

    volk_32fc_magnitude_32f(&d_lambda[low], &d_gamma[low], size);

    // Calculate lambda
    low = 0;
    size = lookup_start - lookup_stop;

    volk_32f_s32f_multiply_32f(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
    volk_32f_x2_subtract_32f(&d_lambda[low], &d_lambda[low], &d_phi[low], size);

    int peak_length, peak, peak_max;
    // Find peaks of lambda
    // We have found an end of symbol at peak_pos[0] + CP + FFT
    if ((peak_length = peak_detect_process(
             &d_lambda[0], (lookup_start - lookup_stop), &peak_pos[0], &peak_max))) {
        peak = peak_pos[peak_max] + lookup_stop;
        *cp_pos = peak;

        // Calculate frequency correction
        float peak_epsilon = fast_atan2f(d_gamma[peak_pos[peak_max]]);
        double sensitivity = (double)(-1) / (double)d_fft_length;

        // Store phases for derotating the signal
        // We always process CP len + FFT len
        for (int i = 0; i < (d_cp_length + d_fft_length); i++) {
            if (i == d_nextpos) {
                d_phaseinc = d_nextphaseinc;
            }

            // We are interested only in fft_length
            d_phase += d_phaseinc;

            while (d_phase > (float)GR_M_PI) {
                d_phase -= (float)(2.0 * GR_M_PI);
            }
            while (d_phase < (float)(-GR_M_PI)) {
                d_phase += (float)(2.0 * GR_M_PI);
            }

            derot[i] = gr_expj(d_phase);
        }

        d_nextphaseinc = sensitivity * peak_epsilon;
        d_nextpos = peak - (d_cp_length + d_fft_length);

        *to_consume = d_cp_length + d_fft_length;
        *to_out = 1;
    } else {
        for (int i = 0; i < (d_cp_length + d_fft_length); i++) {
            d_phase += d_phaseinc;

            while (d_phase > (float)GR_M_PI) {
                d_phase -= (float)(2.0 * GR_M_PI);
            }
            while (d_phase < (float)(-GR_M_PI)) {
                d_phase += (float)(2.0 * GR_M_PI);
            }
        }

        // We consume only fft_length
        *to_consume = d_cp_length + d_fft_length;
        *to_out = 0;
    }

    return (peak_length);
}

void dvbt_ofdm_sym_acquisition_impl::send_sync_start()
{
    const uint64_t offset = this->nitems_written(0);
    pmt::pmt_t key = pmt::string_to_symbol("sync_start");
    pmt::pmt_t value = pmt::from_long(1);
    this->add_item_tag(0, offset, key, value);
}

// Derotates the signal
void dvbt_ofdm_sym_acquisition_impl::derotate(const gr_complex* in, gr_complex* out)
{
    volk_32fc_x2_multiply_32fc(&out[0], &d_derot[0], &in[0], d_fft_length);
}

dvbt_ofdm_sym_acquisition::sptr dvbt_ofdm_sym_acquisition::make(
    int blocks, int fft_length, int occupied_tones, int cp_length, float snr)
{
    return gnuradio::make_block_sptr<dvbt_ofdm_sym_acquisition_impl>(
        blocks, fft_length, occupied_tones, cp_length, snr);
}

/*
 * The private constructor
 */
dvbt_ofdm_sym_acquisition_impl::dvbt_ofdm_sym_acquisition_impl(
    int blocks, int fft_length, int occupied_tones, int cp_length, float snr)
    : block("dvbt_ofdm_sym_acquisition",
            io_signature::make(1, 1, sizeof(gr_complex) * blocks),
            io_signature::make(1, 1, sizeof(gr_complex) * blocks * fft_length)),
      d_fft_length(fft_length),
      d_cp_length(cp_length),
      d_rho(calculate_rho(snr)),
      d_conj(2 * d_fft_length + d_cp_length),
      d_norm(2 * d_fft_length + d_cp_length),
      d_corr(2 * d_fft_length + d_cp_length),
      d_gamma(d_fft_length),
      d_lambda(d_fft_length),
      d_derot(d_fft_length + d_cp_length)
{
    set_relative_rate(1, (uint64_t)(d_cp_length + d_fft_length));
    peak_detect_init(0.3, 0.9);
}

/*
 * Our virtual destructor.
 */
dvbt_ofdm_sym_acquisition_impl::~dvbt_ofdm_sym_acquisition_impl() {}

void dvbt_ofdm_sym_acquisition_impl::forecast(int noutput_items,
                                              gr_vector_int& ninput_items_required)
{
    int ninputs = ninput_items_required.size();

    // make sure we receive at least (symbol_length + fft_length)
    for (int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = (d_cp_length + d_fft_length) * (noutput_items + 1);
    }
}

/*
 * ML Estimation of Time and Frequency Offset in OFDM systems
 * Jan-Jaap van de Beek
 */

int dvbt_ofdm_sym_acquisition_impl::general_work(int noutput_items,
                                                 gr_vector_int& ninput_items,
                                                 gr_vector_const_void_star& input_items,
                                                 gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    int low;

    d_consumed = 0;
    d_out = 0;

    for (int i = 0; i < noutput_items; i++) {
        // This is initial acquisition of symbol start
        // TODO - make a FSM
        if (!d_initial_acquisition) {
            d_initial_acquisition = ml_sync(&in[d_consumed],
                                            2 * d_fft_length + d_cp_length - 1,
                                            d_fft_length + d_cp_length - 1,
                                            &d_cp_start,
                                            &d_derot[0],
                                            &d_to_consume,
                                            &d_to_out);
            d_cp_found = d_initial_acquisition;
        } else {
            // If we are here it means that in the previous iteration we found the CP. We
            // now thus only search near it.
            d_cp_found = ml_sync(&in[d_consumed],
                                 d_cp_start + 8,
                                 std::max(d_cp_start - 8, d_cp_length + d_fft_length - 1),
                                 &d_cp_start,
                                 &d_derot[0],
                                 &d_to_consume,
                                 &d_to_out);
            if (!d_cp_found) {
                // We may have not found the CP because the smaller search range was too
                // small (rare, but possible). We re-try with the whole search range.
                d_cp_found = ml_sync(&in[d_consumed],
                                     2 * d_fft_length + d_cp_length - 1,
                                     d_fft_length + d_cp_length - 1,
                                     &d_cp_start,
                                     &d_derot[0],
                                     &d_to_consume,
                                     &d_to_out);
            }
        }

        if (d_cp_found) {
            low = d_consumed + d_cp_start - d_fft_length + 1;
            derotate(&in[low], &out[i * d_fft_length]);
        } else {
            // Send sync_start downstream
            send_sync_start();
            d_initial_acquisition = 0;

            // Restart with a half number so that we'll not end up with the same situation
            // This will prevent peak_detect to not detect anything
            d_to_consume = d_to_consume / 2;
            d_consumed += d_to_consume;
            consume_each(d_consumed);

            // Tell runtime system how many output items we produced.
            return (d_out);
        }
        d_consumed += d_to_consume;
        d_out += d_to_out;
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(d_to_consume);

    // Tell runtime system how many output items we produced.
    return (d_to_out);
}
} /* namespace dtv */
} /* namespace gr */
