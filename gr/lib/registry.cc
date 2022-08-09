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
    return get_instance()._impls(module, block);
}

std::vector<std::string> registry::parameters(const std::string& module,
                                         const std::string& block)
{
    return get_instance()._parameters(module, block);
}

generic_block_factory registry::factory(const std::string& module,
                                        const std::string& block,
                                        const std::string& impl)
{
    return get_instance()._factory(module, block, impl);
}

parameter_info& registry::parameter(const std::string& module,
                                    const std::string& block,
                                    const std::string& id)
{
    return get_instance()._parameter(module, block, id);
}

registry& registry::register_class(const std::string& module,
                                   const std::string& block,
                                   const std::string& impl,
                                   generic_block_factory factory)
{
    return get_instance()._register_class(module, block, impl, factory);
}


registry& registry::register_parameter(const std::string& module,
                                       const std::string& block,
                                       const parameter_info& info)
{
    return get_instance()._register_parameter(module, block, info);
}


registry::registry() {}

void registry::_init()
{
    std::string prefix("libgnuradio-blocklib");
    // Get the list of shared object files in libdir
    std::string default_lib_dir = libdir();
    std::vector<std::string> dirlist;

    if (const char* env_p = std::getenv("LD_LIBRARY_PATH")) {
        std::string str(env_p);

        char* token = strtok(const_cast<char*>(str.c_str()), ":");
        while (token != nullptr) {
            dirlist.push_back(std::string(token));
            token = strtok(nullptr, ":");
        }
    }
    else {
        dirlist.push_back(default_lib_dir);
    }

    for (auto& d : dirlist) {
        try {
            for (const auto& file : directory_iterator(d)) {
                auto fn = file.path().string();
                auto pos = fn.find(prefix);
                if (pos != std::string::npos) {
                    [[maybe_unused]] void* handle = dlopen(fn.c_str(), RTLD_LAZY);
                }
            }
        } catch (...) {
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

std::vector<std::string> registry::_parameters(const std::string& module,
                                               const std::string& block)
{
    if (!_initialized) {
        _init();
    }
    std::vector<std::string> ret;
    for (const auto& [key, value] : _parameter_map[module][block]) {
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

parameter_info& registry::_parameter(const std::string& module,
                                     const std::string& block,
                                     const std::string& id)
{
    if (!_initialized) {
        _init();
    }
    return _parameter_map[module][block][id];
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


registry& registry::_register_parameter(const std::string& module,
                                        const std::string& block,
                                        const parameter_info& info)
{
    if (_parameter_map.count(module)) {
        if (_parameter_map[module].count(block)) {
            _parameter_map[module][block][info.id] = info;
        }
        else {
            _parameter_map[module][block] = {
                { info.id, info },
            };
        }
    }
    else {
        _parameter_map[module] = {
            { block,
              {
                  { info.id, info },
              } },
        };
    }
    return *this;
}

} // namespace gr