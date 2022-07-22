#pragma once

#include <pmtf/wrap.hpp>
#include <string>

namespace gr {

enum class tag_propagation_policy_t {
    TPP_DONT = 0,       /*!< Scheduler doesn't propagate tags from in- to output. The
                           block itself is free to insert tags as it wants. */
    TPP_ALL_TO_ALL = 1, /*!< Propagate tags from all in- to all outputs. The
                           scheduler takes care of that. */
    TPP_ONE_TO_ONE = 2, /*!< Propagate tags from n. input to n. output. Requires
                           same number of in- and outputs */
    TPP_CUSTOM = 3      /*!< Like TPP_DONT, but signals the block it should implement
                           application-specific forwarding behaviour. */
};

using tag_map = std::map<std::string, pmtf::pmt>;
class tag_t
{
public:
    tag_t() {}
    tag_t(uint64_t offset, std::map<std::string, pmtf::pmt> map)
        : _offset(offset), _map(map)
    {
    }

    tag_t(uint64_t offset, pmtf::pmt map) : _offset(offset), _map(map) {}

    bool operator==(const tag_t& rhs) const;
    bool operator!=(const tag_t& rhs) const;

    void set_offset(uint64_t offset) { _offset = offset; }
    pmtf::pmt operator[](const std::string& key) const { return _map.at(key); }
    uint64_t offset() const { return _offset; }
    pmtf::map map() const { return _map; }

    size_t serialize(std::streambuf& sb) const;
    static tag_t deserialize(std::streambuf& sb);

private:
    uint64_t _offset = 0;
    pmtf::map _map;
};

} // namespace gr
