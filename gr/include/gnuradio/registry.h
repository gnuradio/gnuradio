/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <vector>

#include <gnuradio/block.h>


namespace gr {

using generic_block_factory =
    std::function<std::shared_ptr<block>(std::map<std::string, pmtf::pmt>&)>;

/**
 * @brief Singleton object to register instantiated classes and methods
 *
 */
class registry
{
public:
    static registry& get_instance();

    /**
     * @brief Initializes the registry by finding and loading dynamic modules
     *
     */
    static void init();

    /**
     * @brief Return a list of modules available
     *
     * @return std::vector<std::string>
     */
    static std::vector<std::string> modules();

    /**
     * @brief Return a list of blocks available in the specified module
     *
     * @param module
     * @return std::vector<std::string>
     */
    static std::vector<std::string> blocks(const std::string& module);
    /**
     * @brief Return a list of implementations available for specified block
     *
     * @param module
     * @param block
     * @return std::vector<std::string>
     */
    static std::vector<std::string> impls(const std::string& module,
                                          const std::string& block);

    /**
     * @brief Returns a list of parameters available for specified block
     *
     * @param module
     * @param block
     * @return std::vector<parameter_info>
     */
    static std::vector<std::string> parameters(const std::string& module,
                                               const std::string& block);

    /**
     * @brief Return the generic block factory for a specified block
     *
     * @param module
     * @param block
     * @param impl
     * @return generic_block_factory
     */
    static generic_block_factory factory(const std::string& module,
                                         const std::string& block,
                                         const std::string& impl = "cpu");


    /**
     * @brief Return info about specified parameter
     *
     * @param module
     * @param block
     * @param id
     * @return parameter_info&
     */
    static parameter_info&
    parameter(const std::string& module, const std::string& block, const std::string& id);
    /**
     * @brief Register a block class at initialization time
     *
     * @param module
     * @param block
     * @param impl
     * @param factory
     * @return registry&
     */
    static registry& register_class(const std::string& module,
                                    const std::string& block,
                                    const std::string& impl,
                                    generic_block_factory factory);


    /**
     * @brief Register a parameter at static initialization time
     *
     * @param module
     * @param block
     * @param info
     * @return registry&
     */
    static registry& register_parameter(const std::string& module,
                                        const std::string& block,
                                        const parameter_info& info);

private:
    registry();
    // [module][block][impl] --> generic_block_factory
    std::map<std::string,
             std::map<std::string, std::map<std::string, generic_block_factory>>>
        _constructor_map;
    // [module][block][id] --> parameter_info
    std::map<std::string, std::map<std::string, std::map<std::string, parameter_info>>>
        _parameter_map;

    bool _initialized = false;

    void _init();

    std::vector<std::string> _modules();

    std::vector<std::string> _blocks(const std::string& module);
    std::vector<std::string> _impls(const std::string& module, const std::string& block);
    std::vector<std::string> _parameters(const std::string& module,
                                         const std::string& block);

    generic_block_factory _factory(const std::string& module,
                                   const std::string& block,
                                   const std::string& impl);

    parameter_info& _parameter(const std::string& module,
                               const std::string& block,
                               const std::string& id);

    registry& _register_class(const std::string& module,
                              const std::string& block,
                              const std::string& impl,
                              generic_block_factory factory);

    registry& _register_parameter(const std::string& module,
                                  const std::string& block,
                                  const parameter_info& info);

    // public:
    //     registry(registry const&) = delete;
    //     void operator=(registry const&) = delete;
};

} // namespace gr