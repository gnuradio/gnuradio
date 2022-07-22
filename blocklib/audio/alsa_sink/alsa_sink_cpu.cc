/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "alsa_sink_cpu.h"
#include "alsa_sink_cpu_gen.h"

#include <chrono>
#include <future>
#include <thread>

#include <gnuradio/prefs.h>

namespace gr {
namespace audio {


static bool CHATTY_DEBUG = true;

static snd_pcm_format_t acceptable_formats[] = {
    // these are in our preferred order...
    SND_PCM_FORMAT_S32,
    SND_PCM_FORMAT_S16
};

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

static std::string default_device_name()
{
    return prefs::get_string("audio_alsa", "default_output_device", "default");
}

static double default_period_time()
{
    return std::max(0.001, prefs::get_double("audio_alsa", "period_time", 0.010));
}

static int default_nperiods()
{
    return std::max(2L, prefs::get_long("audio_alsa", "nperiods", 32));
}


alsa_sink_cpu::alsa_sink_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      d_sampling_rate(args.sampling_rate),
      d_device_name(args.device_name.empty() ? default_device_name() : args.device_name),
      d_nperiods(default_nperiods()),
      d_period_time_us((size_t)(default_period_time() * 1e6)),
      d_special_case_mono_to_stereo(false),
      d_ok_to_block(args.ok_to_block),
      d_num_inputs(args.num_inputs)
{
    CHATTY_DEBUG = prefs::get_bool("audio_alsa", "verbose", false);

    int error = -1;
    int dir;

    // open the device for playback
    int attempts = 10;
    while ((error != 0) && (attempts-- > 0)) {
        snd_pcm_t* t = nullptr;
        error = snd_pcm_open(&t, d_device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
        d_pcm_handle.set(t);
        if (error < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    if (args.ok_to_block == false)
        snd_pcm_nonblock(d_pcm_handle.get(), 1);
    if (error < 0) {
        d_logger->error("[{:s}]: {:s}", d_device_name, snd_strerror(error));
        throw std::runtime_error("audio_alsa_sink");
    }

    // Fill params with a full configuration space for a PCM.
    error = snd_pcm_hw_params_any(d_pcm_handle.get(), d_hw_params.get());
    if (error < 0)
        bail("broken configuration for playback", error);

    if (CHATTY_DEBUG)
        gri_alsa_dump_hw_params(d_pcm_handle.get(), d_hw_params.get(), stdout);

    // now that we know how many channels the h/w can handle, set input signature
    unsigned int umin_chan, umax_chan;
    snd_pcm_hw_params_get_channels_min(d_hw_params.get(), &umin_chan);
    snd_pcm_hw_params_get_channels_max(d_hw_params.get(), &umax_chan);
    unsigned int min_chan = std::min(umin_chan, 1000U);
    unsigned int max_chan = std::min(umax_chan, 1000U);

    // As a special case, if the hw's min_chan is two, we'll accept
    // a single input and handle the duplication ourselves.
    if (min_chan == 2) {
        min_chan = 1;
        d_special_case_mono_to_stereo = true;
    }

    if (args.num_inputs < min_chan) {
        throw std::runtime_error(fmt::format(
            "Detected hardware requires at least {} input channels", min_chan));
    }
    if (args.num_inputs > max_chan) {
        throw std::runtime_error(fmt::format(
            "Detected hardware supports no more than {} input channels", max_chan));
    }
    // fill in portions of the d_hw_params that we know now...

    // Specify the access methods we implement
    // For now, we only handle RW_INTERLEAVED...
    snd_pcm_access_mask_t* access_mask;
    snd_pcm_access_mask_t** access_mask_ptr =
        &access_mask; // FIXME: workaround for compiler warning
    snd_pcm_access_mask_alloca(access_mask_ptr);
    snd_pcm_access_mask_none(access_mask);
    snd_pcm_access_mask_set(access_mask, SND_PCM_ACCESS_RW_INTERLEAVED);
    // snd_pcm_access_mask_set(access_mask, SND_PCM_ACCESS_RW_NONINTERLEAVED);

    if ((error = snd_pcm_hw_params_set_access_mask(
             d_pcm_handle.get(), d_hw_params.get(), access_mask)) < 0)
        bail("failed to set access mask", error);

    // set sample format
    if (!gri_alsa_pick_acceptable_format(d_pcm_handle.get(),
                                         d_hw_params.get(),
                                         acceptable_formats,
                                         NELEMS(acceptable_formats),
                                         &d_format,
                                         "audio_alsa_sink",
                                         CHATTY_DEBUG))
        throw std::runtime_error("audio_alsa_sink");

    // sampling rate
    unsigned int orig_sampling_rate = d_sampling_rate;
    if ((error = snd_pcm_hw_params_set_rate_near(
             d_pcm_handle.get(), d_hw_params.get(), &d_sampling_rate, 0)) < 0)
        bail("failed to set rate near", error);

    if (orig_sampling_rate != d_sampling_rate) {
        d_logger->info("[{:s}]: unable to support sampling rate {:d}\n\tCard "
                       "requested {:d} instead.",
                       snd_pcm_name(d_pcm_handle.get()),
                       orig_sampling_rate,
                       d_sampling_rate);
    }

    /*
     * ALSA transfers data in units of "periods".
     * We indirectly determine the underlying buffersize by specifying
     * the number of periods we want (typically 4) and the length of each
     * period in units of time (typically 1ms).
     */
    unsigned int min_nperiods, max_nperiods;
    snd_pcm_hw_params_get_periods_min(d_hw_params.get(), &min_nperiods, &dir);
    snd_pcm_hw_params_get_periods_max(d_hw_params.get(), &max_nperiods, &dir);

    unsigned int orig_nperiods = d_nperiods;
    d_nperiods = std::min(std::max(min_nperiods, d_nperiods), max_nperiods);

    // adjust period time so that total buffering remains more-or-less constant
    d_period_time_us = (d_period_time_us * orig_nperiods) / d_nperiods;

    error = snd_pcm_hw_params_set_periods(
        d_pcm_handle.get(), d_hw_params.get(), d_nperiods, 0);
    if (error < 0)
        bail("set_periods failed", error);

    dir = 0;
    error = snd_pcm_hw_params_set_period_time_near(
        d_pcm_handle.get(), d_hw_params.get(), &d_period_time_us, &dir);
    if (error < 0)
        bail("set_period_time_near failed", error);

    dir = 0;
    error = snd_pcm_hw_params_get_period_size(d_hw_params.get(), &d_period_size, &dir);
    if (error < 0)
        bail("get_period_size failed", error);

    set_output_multiple(d_period_size);
}

bool alsa_sink_cpu::start()
{
    int nchan = d_num_inputs;
    int err;

    // Check the state of the stream
    // Ensure that the pcm is in a state where we can still mess with the hw_params
    snd_pcm_state_t state;
    state = snd_pcm_state(d_pcm_handle.get());
    if (state == SND_PCM_STATE_RUNNING)
        return true; // If stream is running, don't change any parameters
    else if (state == SND_PCM_STATE_XRUN)
        snd_pcm_prepare(
            d_pcm_handle.get()); // Prepare stream on underrun, and we can set parameters;

    bool special_case = nchan == 1 && d_special_case_mono_to_stereo;
    if (special_case)
        nchan = 2;

    err = snd_pcm_hw_params_set_channels(d_pcm_handle.get(), d_hw_params.get(), nchan);

    if (err < 0) {
        output_error_msg("set_channels failed", err);
        return false;
    }

    // set the parameters into the driver...
    err = snd_pcm_hw_params(d_pcm_handle.get(), d_hw_params.get());
    if (err < 0) {
        output_error_msg("snd_pcm_hw_params failed", err);
        return false;
    }

    // get current s/w params
    err = snd_pcm_sw_params_current(d_pcm_handle.get(), d_sw_params.get());
    if (err < 0)
        bail("snd_pcm_sw_params_current", err);

    // Tell the PCM device to wait to start until we've filled
    // it's buffers half way full. This helps avoid audio underruns.

    err = snd_pcm_sw_params_set_start_threshold(
        d_pcm_handle.get(), d_sw_params.get(), d_nperiods * d_period_size / 2);
    if (err < 0)
        bail("snd_pcm_sw_params_set_start_threshold", err);

    // store the s/w params
    err = snd_pcm_sw_params(d_pcm_handle.get(), d_sw_params.get());
    if (err < 0)
        bail("snd_pcm_sw_params", err);

    d_buffer.resize(d_period_size * nchan * snd_pcm_format_size(d_format, 1));

    if (CHATTY_DEBUG) {
        d_debug_logger->debug("[{:s}]: sample resolution = {:d} bits",
                              snd_pcm_name(d_pcm_handle.get()),
                              snd_pcm_hw_params_get_sbits(d_hw_params.get()));
    }

    switch (d_format) {
    case SND_PCM_FORMAT_S16:
        if (special_case) {
            d_worker = [this](work_io& wio) { return this->work_s16_1x2(wio); };
        }
        else {
            d_worker = [this](work_io& wio) { return this->work_s16(wio); };
        }
        break;

    case SND_PCM_FORMAT_S32:
        if (special_case) {
            d_worker = [this](work_io& wio) { return this->work_s32_1x2(wio); };
        }
        else {
            d_worker = [this](work_io& wio) { return this->work_s32(wio); };
        }
        break;

    default:
        throw std::runtime_error("Unsupported PCM format returned from ALSA");
    }

    return true;
}

work_return_code_t alsa_sink_cpu::work(work_io& wio)

{
    // assert((noutput_items % d_period_size) == 0);

    // this is a call through std::function
    return d_worker(wio);
}


/*
 * Work function that deals with float to S16 conversion
 */
work_return_code_t alsa_sink_cpu::work_s16(work_io& wio)
{
    typedef int16_t sample_t; // the type of samples we're creating
    static const float scale_factor = std::pow(2.0f, 16 - 1) - 1;

    size_t nchan = wio.inputs().size();
    auto noutput_items = wio.min_ninput_items();
    std::vector<const float*> in_ptrs(nchan);
    for (size_t i = 0; i < wio.inputs().size(); i++) {
        in_ptrs[i] = wio.inputs()[i].items<float>();
    }
    auto in = in_ptrs.data();
    sample_t* buf = reinterpret_cast<sample_t*>(d_buffer.data());
    int bi;
    size_t n;

    size_t sizeof_frame = nchan * sizeof(sample_t);
    assert(d_buffer.size() == d_period_size * sizeof_frame);

    for (n = 0; n < noutput_items; n += d_period_size) {
        // process one period of data
        bi = 0;
        for (size_t i = 0; i < d_period_size; i++) {
            for (size_t chan = 0; chan < nchan; chan++) {
                buf[bi++] = (sample_t)(in[chan][i] * scale_factor);
            }
        }

        // update src pointers
        for (size_t chan = 0; chan < nchan; chan++)
            in[chan] += d_period_size;

        if (!write_buffer(buf, d_period_size, sizeof_frame))
            return work_return_code_t::WORK_DONE; // No fixing this problem.  Say
                                                  // we're done.
    }

    wio.consume_each(n);
    return work_return_code_t::WORK_OK;
}

/*
 * Work function that deals with float to S32 conversion
 */
work_return_code_t alsa_sink_cpu::work_s32(work_io& wio)
{
    typedef int32_t sample_t; // the type of samples we're creating
    static const float scale_factor = std::pow(2.0f, 32 - 1) - 1;

    size_t nchan = wio.inputs().size();
    auto noutput_items = wio.min_ninput_items();
    std::vector<const float*> in_ptrs(nchan);
    for (size_t i = 0; i < wio.inputs().size(); i++) {
        in_ptrs[i] = wio.inputs()[i].items<float>();
    }
    auto in = in_ptrs.data();
    sample_t* buf = reinterpret_cast<sample_t*>(d_buffer.data());
    int bi;
    size_t n;

    size_t sizeof_frame = nchan * sizeof(sample_t);
    assert(d_buffer.size() == d_period_size * sizeof_frame);

    for (n = 0; n < noutput_items; n += d_period_size) {
        // process one period of data
        bi = 0;
        for (size_t i = 0; i < d_period_size; i++) {
            for (size_t chan = 0; chan < nchan; chan++) {
                buf[bi++] = (sample_t)(in[chan][i] * scale_factor);
            }
        }

        // update src pointers
        for (size_t chan = 0; chan < nchan; chan++)
            in[chan] += d_period_size;

        if (!write_buffer(buf, d_period_size, sizeof_frame))
            return work_return_code_t::WORK_DONE; // No fixing this problem.  Say
                                                  // we're done.
    }

    wio.consume_each(n);
    return work_return_code_t::WORK_OK;
}

/*
 * Work function that deals with float to S16 conversion and
 * mono to stereo kludge.
 */
work_return_code_t alsa_sink_cpu::work_s16_1x2(work_io& wio)
{
    typedef int16_t sample_t; // the type of samples we're creating
    static const float scale_factor = std::pow(2.0f, 16 - 1) - 1;

    assert(wio.inputs().size() == 1);
    static const size_t nchan = 2;
    auto noutput_items = wio.min_ninput_items();
    std::vector<const float*> in_ptrs(nchan);
    for (size_t i = 0; i < wio.inputs().size(); i++) {
        in_ptrs[i] = wio.inputs()[i].items<float>();
    }
    auto in = in_ptrs.data();
    sample_t* buf = reinterpret_cast<sample_t*>(d_buffer.data());
    int bi;
    size_t n;

    size_t sizeof_frame = nchan * sizeof(sample_t);
    assert(d_buffer.size() == d_period_size * sizeof_frame);

    for (n = 0; n < noutput_items; n += d_period_size) {
        // process one period of data
        bi = 0;
        for (size_t i = 0; i < d_period_size; i++) {
            sample_t t = (sample_t)(in[0][i] * scale_factor);
            buf[bi++] = t;
            buf[bi++] = t;
        }

        // update src pointers
        in[0] += d_period_size;

        if (!write_buffer(buf, d_period_size, sizeof_frame))
            return work_return_code_t::WORK_DONE; // No fixing this problem.  Say
                                                  // we're done.
    }

    wio.consume_each(n);
    return work_return_code_t::WORK_OK;
}

/*
 * Work function that deals with float to S32 conversion and
 * mono to stereo kludge.
 */
work_return_code_t alsa_sink_cpu::work_s32_1x2(work_io& wio)
{
    typedef int32_t sample_t; // the type of samples we're creating
    static const float scale_factor = std::pow(2.0f, 32 - 1) - 1;

    static size_t nchan = 2;
    auto noutput_items = wio.min_ninput_items();
    std::vector<const float*> in_ptrs(nchan);
    for (size_t i = 0; i < wio.inputs().size(); i++) {
        in_ptrs[i] = wio.inputs()[i].items<float>();
    }
    auto in = in_ptrs.data();
    sample_t* buf = reinterpret_cast<sample_t*>(d_buffer.data());
    int bi;
    size_t n;

    size_t sizeof_frame = nchan * sizeof(sample_t);
    assert(d_buffer.size() == d_period_size * sizeof_frame);

    for (n = 0; n < noutput_items; n += d_period_size) {
        // process one period of data
        bi = 0;
        for (size_t i = 0; i < d_period_size; i++) {
            sample_t t = (sample_t)(in[0][i] * scale_factor);
            buf[bi++] = t;
            buf[bi++] = t;
        }

        // update src pointers
        in[0] += d_period_size;

        if (!write_buffer(buf, d_period_size, sizeof_frame))
            return work_return_code_t::WORK_DONE; // No fixing this problem.  Say
                                                  // we're done.
    }

    wio.consume_each(n);
    return work_return_code_t::WORK_OK;
}

bool alsa_sink_cpu::write_buffer(const void* vbuffer,
                                 unsigned nframes,
                                 unsigned sizeof_frame)
{
    const unsigned char* buffer = (const unsigned char*)vbuffer;

    while (nframes > 0) {
        int r = snd_pcm_writei(d_pcm_handle.get(), buffer, nframes);
        if (r == -EAGAIN) {
            if (d_ok_to_block == true)
                continue; // try again
            break;
        }
        else if (r == -EPIPE) { // underrun
            d_nunderuns++;
            // we need to have an lvalue, async pitfall!
            auto future_local = std::async(::fputs, "aU", stderr);

            if ((r = snd_pcm_prepare(d_pcm_handle.get())) < 0) {
                output_error_msg("snd_pcm_prepare failed. Can't recover from underrun",
                                 r);
                return false;
            }
            continue; // try again
        }
#ifdef ESTRPIPE
        else if (r == -ESTRPIPE) { // h/w is suspended (whatever that means)
                                   // This is apparently related to power management
            d_nsuspends++;
            if ((r = snd_pcm_resume(d_pcm_handle.get())) < 0) {
                output_error_msg("failed to resume from suspend", r);
                return false;
            }
            continue; // try again
        }
#endif
        else if (r < 0) {
            output_error_msg("snd_pcm_writei failed", r);
            return false;
        }

        nframes -= r;
        buffer += r * sizeof_frame;
    }

    return true;
}

void alsa_sink_cpu::output_error_msg(const char* msg, int err)
{
    d_logger->error(
        "[{:s}]: {:s}: {:s}", snd_pcm_name(d_pcm_handle.get()), msg, snd_strerror(err));
}

void alsa_sink_cpu::bail(const char* msg, int err)
{
    output_error_msg(msg, err);
    throw std::runtime_error("audio_alsa_sink");
}


} // namespace audio
} // namespace gr