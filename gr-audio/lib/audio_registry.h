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

#include <gnuradio/audio/sink.h>
#include <gnuradio/audio/source.h>
#include <string>

namespace gr {
  namespace audio {

    typedef source::sptr(*source_factory_t)(int, const std::string &, bool);
    typedef sink::sptr(*sink_factory_t)(int, const std::string &, bool);

    enum reg_prio_type {
      REG_PRIO_LOW = 100,
      REG_PRIO_MED = 200,
      REG_PRIO_HIGH = 300
    };

    void register_source(reg_prio_type prio, const std::string &arch,
                         source_factory_t source);
    void register_sink(reg_prio_type prio, const std::string &arch,
                       sink_factory_t sink);

#define AUDIO_REGISTER_FIXTURE(x) static struct x{x();}x;x::x()

#define AUDIO_REGISTER_SOURCE(prio, arch) \
    static source::sptr arch##_source_fcn(int, const std::string &, bool); \
    AUDIO_REGISTER_FIXTURE(arch##_source_reg) {                   \
      register_source(prio, #arch, &arch##_source_fcn);           \
    } static source::sptr arch##_source_fcn

#define AUDIO_REGISTER_SINK(prio, arch)                            \
    static sink::sptr arch##_sink_fcn(int, const std::string &, bool); \
    AUDIO_REGISTER_FIXTURE(arch##_sink_reg) {                     \
      register_sink(prio, #arch, &arch##_sink_fcn);               \
    } static sink::sptr arch##_sink_fcn

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_GR_AUDIO_REGISTRY_H */
