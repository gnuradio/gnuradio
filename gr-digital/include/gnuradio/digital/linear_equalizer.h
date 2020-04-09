/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_LINEAR_EQUALIZER_H
#define INCLUDED_DIGITAL_LINEAR_EQUALIZER_H

#include <gnuradio/digital/adaptive_algorithm.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/sync_decimator.h>

#include <string>
#include <vector>

namespace gr {
namespace digital {

/*!
 * \brief Linear Equalizer block provides linear equalization using a specified adaptive
 * algorithm \ingroup equalizers
 *
 */
class DIGITAL_API linear_equalizer : virtual public gr::sync_decimator
{

public:
    typedef std::shared_ptr<linear_equalizer> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * gr::digital::linear_equalizer.
     *
     * The Linear Equalizer block equalizes the incoming signal using an FIR filter.
     * If provided with a training sequence and a training start tag, data aided
     * equalization will be performed starting with the tagged sample. If training-based
     * equalization is active and the training sequence ends, then optionally decision
     * directed equalization will be performed given the adapt_after_training If no
     * training sequence or no tag is provided, decision directed equalization will be
     * performed
     * This equalizer decimates to the symbol rate according to the samples per symbol
     * param
     *
     * \param num_taps  Number of taps for the FIR filter
     * \param sps  int - Samples per Symbol
     * \param alg  Adaptive algorithm object
     * \param training_sequence  Sequence of samples that will be used to train the
     * equalizer.  Provide empty vector to default to DD equalizer
     * \param adapt_after_training  bool - set true to continue DD training after training
     * sequence has been used up
     * \param training_start_tag  string - tag that indicates the start
     * of the training sequence in the incoming data
     */
    static sptr
    make(unsigned num_taps,
         unsigned sps,
         adaptive_algorithm_sptr alg,
         bool adapt_after_training = true,
         std::vector<gr_complex> training_sequence = std::vector<gr_complex>(),
         const std::string& training_start_tag = "");

    virtual void set_taps(const std::vector<gr_complex>& taps) = 0;
    virtual std::vector<gr_complex> taps() const = 0;

    /*!
     * \brief Public "work" function - equalize a block of input samples
     * \details Behaves similar to the block's work function, but made public
     * to be able to be called outside the GNU Radio scheduler on bursty data
     * \param input_samples   Buffer of input samples to equalize
     * \param output_symbols  Buffer of output symbols post equalization
     * \param num_inputs      Number of input samples provided
     * \param max_num_outputs Size of output_symbols buffer to write into
     * \param training_start_samples Vector of starting positions of training sequences
     * within the input_samples buffer
     * \param history_included  Flag to indicate whether history has been provided at
     * the beginning of the input_samples buffer, as would normally be provided in a
     * work() call.  The work() function of this block sets this to true, but in bursty
     * operation, this should be set to false
     * \param taps            Optional output vector buffer of tap weights calculated at
     * each sample of the output
     * \param state           Optional output state of the equalizer for debug {IDLE,
     * TRAINING, DD}
     */
    virtual int equalize(
        const gr_complex* input_samples,
        gr_complex* output_symbols,
        unsigned int num_inputs,
        unsigned int max_num_outputs,
        std::vector<unsigned int> training_start_samples = std::vector<unsigned int>(0),
        bool history_included = false,
        gr_complex* taps = nullptr,
        unsigned short* state = nullptr) = 0;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_LINEAR_EQUALIZER_H */
