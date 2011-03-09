/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_AUDIO_REGISTRY_H
#define INCLUDED_GR_AUDIO_REGISTRY_H

#include <gr_audio_sink.h>
#include <gr_audio_source.h>
#include <string>

typedef audio_source::sptr(*source_factory_t)(int, const std::string &, bool);
typedef audio_sink::sptr(*sink_factory_t)(int, const std::string &, bool);

void audio_register_source(const std::string &name, source_factory_t source);
void audio_register_sink(const std::string &name, sink_factory_t sink);

#define AUDIO_REGISTER_FIXTURE(x) static struct x{x();}x;x::x()

#define AUDIO_REGISTER_SOURCE(name) \
    static audio_source::sptr name##_source_fcn(int, const std::string &, bool); \
    AUDIO_REGISTER_FIXTURE(name##_source_reg){ \
        audio_register_source(#name, &name##_source_fcn); \
    } static audio_source::sptr name##_source_fcn

#define AUDIO_REGISTER_SINK(name) \
    static audio_sink::sptr name##_sink_fcn(int, const std::string &, bool); \
    AUDIO_REGISTER_FIXTURE(name##_sink_reg){ \
        audio_register_sink(#name, &name##_sink_fcn); \
    } static audio_sink::sptr name##_sink_fcn

#endif /* INCLUDED_GR_AUDIO_REGISTRY_H */
