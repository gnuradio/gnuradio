/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#include "audio_registry.h"
#include <boost/foreach.hpp>
#include <gnuradio/prefs.h>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace gr {
  namespace audio {

    /***********************************************************************
     * Create registries
     **********************************************************************/

    struct source_entry_t {
      reg_prio_type prio;
      std::string arch;
      source_factory_t source;
    };

    static std::vector<source_entry_t> &get_source_registry(void)
    {
      static std::vector<source_entry_t> d_registry;
      return d_registry;
    }

    struct sink_entry_t
    {
      reg_prio_type prio;
      std::string arch;
      sink_factory_t sink;
    };

    static std::vector<sink_entry_t> &get_sink_registry(void)
    {
      static std::vector<sink_entry_t> d_registry;
      return d_registry;
    }

    /***********************************************************************
     * Register functions
     **********************************************************************/
    void
    register_source(reg_prio_type prio,
                    const std::string &arch,
                    source_factory_t source)
    {
      source_entry_t entry;
      entry.prio = prio;
      entry.arch = arch;
      entry.source = source;
      get_source_registry().push_back(entry);
    }

    void register_sink(reg_prio_type prio,
                       const std::string &arch,
                       sink_factory_t sink)
    {
      sink_entry_t entry;
      entry.prio = prio;
      entry.arch = arch;
      entry.sink = sink;
      get_sink_registry().push_back(entry);
    }

    /***********************************************************************
     * Factory functions
     **********************************************************************/
    static std::string default_arch_name(void)
    {
      return prefs::singleton()->get_string("audio", "audio_module", "auto");
    }

    static void do_arch_warning(const std::string &arch)
    {
      if(arch == "auto")
        return; //no warning when arch not specified
      std::cerr << "Could not find audio architecture \"" << arch
                << "\" in registry." << std::endl;
      std::cerr << "    Defaulting to the first available architecture..." << std::endl;
    }

    source::sptr
    source::make(int sampling_rate,
                 const std::string device_name,
                 bool ok_to_block)
    {
      if(get_source_registry().empty()) {
        throw std::runtime_error("no available audio source factories");
      }

      std::string arch = default_arch_name();
      source_entry_t entry = get_source_registry().front();

      BOOST_FOREACH(const source_entry_t &e, get_source_registry()) {
        if(e.prio > entry.prio)
          entry = e; //entry is highest prio
        if(arch != e.arch)
          continue; //continue when no match
        return e.source(sampling_rate, device_name, ok_to_block);
      }

      //std::cout << "Audio source arch: " << entry.name << std::endl;
      return entry.source(sampling_rate, device_name, ok_to_block);
    }

    sink::sptr
    sink::make(int sampling_rate,
               const std::string device_name,
               bool ok_to_block)
    {
      if(get_sink_registry().empty()) {
        throw std::runtime_error("no available audio sink factories");
      }

      std::string arch = default_arch_name();
      sink_entry_t entry = get_sink_registry().front();

      BOOST_FOREACH(const sink_entry_t &e, get_sink_registry()) {
        if(e.prio > entry.prio)
          entry = e; //entry is highest prio
        if(arch != e.arch)
          continue; //continue when no match
        return e.sink(sampling_rate, device_name, ok_to_block);
      }

      do_arch_warning(arch);
      //std::cout << "Audio sink arch: " << entry.name << std::endl;
      return entry.sink(sampling_rate, device_name, ok_to_block);
    }

  } /* namespace audio */
} /* namespace gr */
