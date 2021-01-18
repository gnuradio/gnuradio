/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ALSA_IMPL_H
#define INCLUDED_ALSA_IMPL_H

#include <alsa/asoundlib.h>
#include <cstdio>

void gri_alsa_dump_hw_params(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwparams, FILE* fp);

bool gri_alsa_pick_acceptable_format(snd_pcm_t* pcm,
                                     snd_pcm_hw_params_t* hwparams,
                                     snd_pcm_format_t acceptable_formats[],
                                     unsigned nacceptable_formats,
                                     snd_pcm_format_t* selected_format,
                                     const char* error_msg_tag,
                                     bool verbose);

namespace gr {
namespace audio {
namespace alsa_internal {
// hw/sw params RAII wrapper.
template <typename T, int Alloc(T**), void Free(T*)>
class param_wrap
{
public:
    param_wrap()
    {
        const int err = Alloc(&d_param);
        if (err) {
            throw std::runtime_error("failed to allocate ALSA params. Error code " +
                                     std::to_string(err));
        }
    }
    param_wrap(const param_wrap&) = delete;
    param_wrap& operator=(const param_wrap&) = delete;
    T* get() { return d_param; }
    ~param_wrap() { Free(d_param); }

private:
    T* d_param = nullptr;
};
typedef param_wrap<snd_pcm_hw_params_t, snd_pcm_hw_params_malloc, snd_pcm_hw_params_free>
    hwparam_wrap;
typedef param_wrap<snd_pcm_sw_params_t, snd_pcm_sw_params_malloc, snd_pcm_sw_params_free>
    swparam_wrap;

class sndpcm_wrap
{
public:
    sndpcm_wrap(snd_pcm_t* in = nullptr) : d_pcm_handle(in) {}
    sndpcm_wrap(const sndpcm_wrap&) = delete;
    sndpcm_wrap(sndpcm_wrap&&) = delete;
    sndpcm_wrap& operator=(const sndpcm_wrap&) = delete;
    sndpcm_wrap& operator=(sndpcm_wrap&&) = delete;
    ~sndpcm_wrap() { close(); }
    void close()
    {
        if (d_pcm_handle == nullptr) {
            return;
        }
        if (snd_pcm_state(d_pcm_handle) == SND_PCM_STATE_RUNNING) {
            snd_pcm_drop(d_pcm_handle);
        }
        snd_pcm_close(d_pcm_handle);
        d_pcm_handle = nullptr;
    }
    void set(snd_pcm_t* ptr)
    {
        close();
        d_pcm_handle = ptr;
    }
    snd_pcm_t* get() noexcept { return d_pcm_handle; }

private:
    snd_pcm_t* d_pcm_handle;
};

} // namespace alsa_internal
} // namespace audio
} // namespace gr

#endif /* INCLUDED_GRI_ALSA_H */
