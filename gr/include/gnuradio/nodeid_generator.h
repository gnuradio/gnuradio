#pragma once

#include <string>
#include <vector>

namespace gr {

/**
 * @brief Singleton object to produce unique block ids
 *
 * The logic of how nodeids are generated is very simple, just an incrementing integer so
 * that all blocks across the flowgraph are given a unique ID that can be used for map
 * indices and other references
 *
 */

class nodeid_generator
{
public:
    static nodeid_generator& get_instance();

    /**
     * @brief Get the next global id
     *
     * Static method to return a fresh global id when a node is instantiated
     *
     * @return uint32_t the global id
     */
    static uint32_t get_id();
    static std::string get_unique_string();

private:
    std::vector<uint32_t> _used_ids;
    std::vector<std::string> _used_strings;
    uint32_t _last_id = 0;
    nodeid_generator() {}

    uint32_t get_id_();
    std::string get_unique_string_();

public:
    nodeid_generator(nodeid_generator const&) = delete;
    void operator=(nodeid_generator const&) = delete;
};

} // namespace gr
