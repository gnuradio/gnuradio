/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_PORTAUDIO_IMPL_H
#define INCLUDED_AUDIO_PORTAUDIO_IMPL_H

#include <portaudio.h>
#include <cstdio>

namespace gr {
namespace audio {

PaDeviceIndex pa_find_device_by_name(const char* name);
void print_devices();

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_PORTAUDIO_IMPL_H */
