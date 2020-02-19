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
#include <stdio.h>

void gri_alsa_dump_hw_params(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwparams, FILE* fp);

bool gri_alsa_pick_acceptable_format(snd_pcm_t* pcm,
                                     snd_pcm_hw_params_t* hwparams,
                                     snd_pcm_format_t acceptable_formats[],
                                     unsigned nacceptable_formats,
                                     snd_pcm_format_t* selected_format,
                                     const char* error_msg_tag,
                                     bool verbose);


#endif /* INCLUDED_GRI_ALSA_H */
