/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_WAVFILE_SINK_IMPL_H
#define INCLUDED_GR_WAVFILE_SINK_IMPL_H

#include <gnuradio/blocks/wavfile.h>
#include <gnuradio/blocks/wavfile_sink.h>
#include <gnuradio/thread/thread.h>
#include <sndfile.h> // for SNDFILE

namespace gr {
namespace blocks {

class wavfile_sink_impl : public wavfile_sink
{
private:
    wav_header_info d_h;
    bool d_append;

    std::vector<float> d_buffer;

    SNDFILE* d_fp;
    SNDFILE* d_new_fp;
    bool d_updated;
    gr::thread::mutex d_mutex;

    bool d_should_reopen = false;
    std::string d_filename;

    static constexpr int s_items_size = 8192;
    static constexpr int s_max_channels = 24;

    /*!
     * \brief If any file changes have occurred, update now. This is called
     * internally by work() and thus doesn't usually need to be called by
     * hand.
     */
    void do_update();

    /*!
     * \brief Writes information to the WAV header which is not available
     * a-priori (chunk size etc.) and closes the file. Not thread-safe and
     * assumes d_fp is a valid file pointer, should thus only be called by
     * other methods.
     */
    void close_wav();

    bool open_file_for_append();
    bool open_file_for_rewrite();

protected:
    bool stop() override;
    bool start() override;

public:
    wavfile_sink_impl(const char* filename,
                      int n_channels,
                      unsigned int sample_rate,
                      wavfile_format_t format,
                      wavfile_subformat_t subformat,
                      bool append);
    ~wavfile_sink_impl() override;

    bool open(const char* filename) override;
    void close() override;

    void set_sample_rate(unsigned int sample_rate) override;
    void set_bits_per_sample(int bits_per_sample) override;
    void set_append(bool append) override;

    int bits_per_sample();
    unsigned int sample_rate();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SINK_IMPL_H */
