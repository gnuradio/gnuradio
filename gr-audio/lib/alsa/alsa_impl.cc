/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#warning "ALSA CONFIG H"
#include "config.h"
#endif

#include <gnuradio/logger.h>

#include "alsa_impl.h"
#include <algorithm>

static snd_pcm_access_t access_types[] = { SND_PCM_ACCESS_MMAP_INTERLEAVED,
                                           SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
                                           SND_PCM_ACCESS_MMAP_COMPLEX,
                                           SND_PCM_ACCESS_RW_INTERLEAVED,
                                           SND_PCM_ACCESS_RW_NONINTERLEAVED };

static snd_pcm_format_t format_types[] = {
    // SND_PCM_FORMAT_UNKNOWN,
    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S16_BE,
    SND_PCM_FORMAT_U16_LE,
    SND_PCM_FORMAT_U16_BE,
    SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_S24_BE,
    SND_PCM_FORMAT_U24_LE,
    SND_PCM_FORMAT_U24_BE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_U32_LE,
    SND_PCM_FORMAT_U32_BE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT_BE,
    SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
    SND_PCM_FORMAT_MU_LAW,
    SND_PCM_FORMAT_A_LAW,
    SND_PCM_FORMAT_IMA_ADPCM,
    SND_PCM_FORMAT_MPEG,
    SND_PCM_FORMAT_GSM,
    SND_PCM_FORMAT_SPECIAL,
    SND_PCM_FORMAT_S24_3LE,
    SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_U24_3LE,
    SND_PCM_FORMAT_U24_3BE,
    SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_U20_3LE,
    SND_PCM_FORMAT_U20_3BE,
    SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_U18_3LE,
    SND_PCM_FORMAT_U18_3BE
};

static unsigned int test_rates[] = { 8000,  16000, 22050, 32000,
                                     44100, 48000, 96000, 192000 };

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

void gri_alsa_dump_hw_params(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwparams, FILE* fp)
{
    fprintf(fp, "PCM name: %s\n", snd_pcm_name(pcm));

    fprintf(fp, "Access types:\n");
    for (unsigned i = 0; i < NELEMS(access_types); i++) {
        snd_pcm_access_t at = access_types[i];
        fprintf(fp,
                "    %-20s %s\n",
                snd_pcm_access_name(at),
                snd_pcm_hw_params_test_access(pcm, hwparams, at) == 0 ? "YES" : "NO");
    }

    fprintf(fp, "Formats:\n");
    for (unsigned i = 0; i < NELEMS(format_types); i++) {
        snd_pcm_format_t ft = format_types[i];
        if (0)
            fprintf(fp,
                    "    %-20s %s\n",
                    snd_pcm_format_name(ft),
                    snd_pcm_hw_params_test_format(pcm, hwparams, ft) == 0 ? "YES" : "NO");
        else {
            if (snd_pcm_hw_params_test_format(pcm, hwparams, ft) == 0)
                fprintf(fp, "    %-20s YES\n", snd_pcm_format_name(ft));
        }
    }

    fprintf(fp, "Number of channels\n");
    unsigned int min_chan, max_chan;
    snd_pcm_hw_params_get_channels_min(hwparams, &min_chan);
    snd_pcm_hw_params_get_channels_max(hwparams, &max_chan);
    fprintf(fp, "    min channels: %d\n", min_chan);
    fprintf(fp, "    max channels: %d\n", max_chan);
    unsigned int chan;
    max_chan = std::min(max_chan, 16U); // truncate display...
    for (chan = min_chan; chan <= max_chan; chan++) {
        fprintf(fp,
                "    %d channels\t%s\n",
                chan,
                snd_pcm_hw_params_test_channels(pcm, hwparams, chan) == 0 ? "YES" : "NO");
    }

    fprintf(fp, "Sample Rates:\n");
    unsigned int min_rate, max_rate;
    int min_dir, max_dir;

    snd_pcm_hw_params_get_rate_min(hwparams, &min_rate, &min_dir);
    snd_pcm_hw_params_get_rate_max(hwparams, &max_rate, &max_dir);
    fprintf(fp, "    min rate: %7d (dir = %d)\n", min_rate, min_dir);
    fprintf(fp, "    max rate: %7d (dir = %d)\n", max_rate, max_dir);
    for (unsigned i = 0; i < NELEMS(test_rates); i++) {
        unsigned int rate = test_rates[i];
        fprintf(fp,
                "    %6u  %s\n",
                rate,
                snd_pcm_hw_params_test_rate(pcm, hwparams, rate, 0) == 0 ? "YES" : "NO");
    }

    fflush(fp);
}

bool gri_alsa_pick_acceptable_format(snd_pcm_t* pcm,
                                     snd_pcm_hw_params_t* hwparams,
                                     snd_pcm_format_t acceptable_formats[],
                                     unsigned nacceptable_formats,
                                     snd_pcm_format_t* selected_format,
                                     const char* error_msg_tag,
                                     bool verbose)
{
    int err;
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(
        logger, debug_logger, "gri_alsa_pick_acceptable_format");

    // pick a format that we like...
    for (unsigned i = 0; i < nacceptable_formats; i++) {
        if (snd_pcm_hw_params_test_format(pcm, hwparams, acceptable_formats[i]) == 0) {
            err = snd_pcm_hw_params_set_format(pcm, hwparams, acceptable_formats[i]);
            if (err < 0) {
                GR_LOG_ERROR(logger,
                             boost::format("%s[%s]: failed to set format: %s") %
                                 error_msg_tag % snd_pcm_name(pcm) % snd_strerror(err));
                return false;
            }
            if (verbose)
                fprintf(stdout,
                        "%s[%s]: using %s",
                        error_msg_tag,
                        snd_pcm_name(pcm),
                        snd_pcm_format_name(acceptable_formats[i]));
            *selected_format = acceptable_formats[i];
            return true;
        }
    }

    GR_LOG_ERROR(logger,
                 boost::format("%s[%s]: failed to find acceptable format") %
                     error_msg_tag % snd_pcm_name(pcm));
    return false;
}
