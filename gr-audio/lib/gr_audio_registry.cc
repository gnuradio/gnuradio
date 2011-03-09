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

#include "gr_audio_registry.h"
#include <stdexcept>
#include <vector>

static std::vector<gr_audio_registry_entry> &get_registry(void){
    static std::vector<gr_audio_registry_entry> _registry;
    return _registry;
}

void gr_audio_register(const gr_audio_registry_entry &entry){
    get_registry().push_back(entry);
}

gr_audio_source::sptr gr_make_audio_source(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_registry().empty()){
        throw std::runtime_error("no available audio factories");
    }
    //TODO we may prefer to use a specific entry in the registry
    return get_registry().front().source(sampling_rate, device_name, ok_to_block);
}

gr_audio_sink::sptr gr_make_audio_sink(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_registry().empty()){
        throw std::runtime_error("no available audio factories");
    }
    //TODO we may prefer to use a specific entry in the registry
    return get_registry().front().sink(sampling_rate, device_name, ok_to_block);
}
