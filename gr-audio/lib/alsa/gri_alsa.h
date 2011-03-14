/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRI_ALSA_H
#define INCLUDED_GRI_ALSA_H

#include <stdio.h>
#include <alsa/asoundlib.h>

void 
gri_alsa_dump_hw_params (snd_pcm_t *pcm,
			 snd_pcm_hw_params_t *hwparams,
			 FILE *fp);

bool
gri_alsa_pick_acceptable_format (snd_pcm_t *pcm,
				 snd_pcm_hw_params_t *hwparams,
				 snd_pcm_format_t acceptable_formats[],
				 unsigned nacceptable_formats,
				 snd_pcm_format_t *selected_format,
				 const char *error_msg_tag,
				 bool verbose);


#endif /* INCLUDED_GRI_ALSA_H */
