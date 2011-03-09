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

struct gr_audio_registry_entry{
    typedef gr_audio_source::sptr(*source_factory_t)(int, const std::string &, bool);
    typedef gr_audio_sink::sptr(*sink_factory_t)(int, const std::string &, bool);
    std::string name;
    source_factory_t source;
    sink_factory_t sink;
};

void gr_audio_register(const gr_audio_registry_entry &entry);

#endif /* INCLUDED_GR_AUDIO_REGISTRY_H */
