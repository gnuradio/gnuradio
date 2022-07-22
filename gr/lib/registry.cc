#include <gnuradio/registry.h>

/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include <functional>

#include <gnuradio/block.h>
#include <gnuradio/constants.h>

#include <dlfcn.h>
#include <filesystem>

using std::filesystem::directory_iterator;

namespace gr {


registry& registry::get_instance()
{
    static registry instance;
    return instance;
}

void registry::init() { get_instance()._init(); }

std::vector<std::string> registry::modules() { return get_instance()._modules(); }

std::vector<std::string> registry::blocks(const std::string& module)
{
    return get_instance()._blocks(module);
}

std::vector<std::string> registry::impls(const std::string& module,
                                         const std::string& block)
{
    return get_instance().impls(module, block);
}

generic_block_factory registry::factory(const std::string& module,
                                        const std::string& block,
                                        const std::string& impl)
{
    return get_instance()._factory(module, block, impl);
}

registry& registry::register_class(const std::string& module,
                                   const std::string& block,
                                   const std::string& impl,
                                   generic_block_factory factory)
{
    return get_instance()._register_class(module, block, impl, factory);
}


registry::registry() {}

void registry::_init()
{
    std::string prefix("libgnuradio-blocklib");
    // Get the list of shared object files in libdir
    for (const auto& file : directory_iterator(libdir())) {
        auto fn = file.path().string();
        auto pos = fn.find(prefix);
        if (pos != std::string::npos) {
            [[maybe_unused]] void* handle = dlopen(fn.c_str(), RTLD_LAZY);
        }
    }

    _initialized = true;
}

std::vector<std::string> registry::_modules()
{
    if (!_initialized) {
        _init();
    }
    std::vector<std::string> ret;
    for (const auto& [key, value] : _constructor_map) {
        ret.push_back(key);
    }
    return ret;
}

std::vector<std::string> registry::_blocks(const std::string& module)
{
    if (!_initialized) {
        _init();
    }
    std::vector<std::string> ret;
    for (const auto& [key, value] : _constructor_map[module]) {
        ret.push_back(key);
    }
    return ret;
}
std::vector<std::string> registry::_impls(const std::string& module,
                                          const std::string& block)
{
    if (!_initialized) {
        _init();
    }
    std::vector<std::string> ret;
    for (const auto& [key, value] : _constructor_map[module][block]) {
        ret.push_back(key);
    }
    return ret;
}

generic_block_factory registry::_factory(const std::string& module,
                                         const std::string& block,
                                         const std::string& impl)
{
    if (!_initialized) {
        _init();
    }
    return _constructor_map[module][block][impl];
}

registry& registry::_register_class(const std::string& module,
                                    const std::string& block,
                                    const std::string& impl,
                                    generic_block_factory factory)
{
    if (_constructor_map.count(module)) {
        if (_constructor_map[module].count(block)) {
            _constructor_map[module][block][impl] = factory;
        }
        else {
            _constructor_map[module][block] = {
                { impl, factory },
            };
        }
    }
    else {
        _constructor_map[module] = {
            { block,
              {
                  { impl, factory },
              } },
        };
    }
    return *this;
}


} // namespace gr