#pragma once

#include <pmtv/pmt.hpp>
#include <map>
#include <optional>
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

using tag_map = std::map<std::string, pmtv::pmt>;
class tag_t
{
public:
    tag_t() {}
    tag_t(uint64_t offset, const tag_map& map) : _offset(offset), _map(map) {}

    tag_t(uint64_t offset, pmtv::pmt map) : _offset(offset), _map(pmtv::get_map(map)) {}

    bool operator==(const tag_t& rhs) const;
    bool operator!=(const tag_t& rhs) const;

    void set_offset(uint64_t offset) { _offset = offset; }
    [[deprecated("unsafe - use at(key) instead")]] pmtv::pmt
    operator[](const std::string& key) const
    {
        return _map.at(key); // implementation violates std::map::operator[] contract
    }

    pmtv::pmt& at(const std::string& key) { return _map.at(key); }
    const pmtv::pmt& at(const std::string& key) const { return _map.at(key); }

    [[nodiscard]] std::optional<std::reference_wrapper<const pmtv::pmt>>
    get(const std::string& key) const noexcept
    {
        try { // TODO: missing find function in pmtv::map -> exception based look-up
            return _map.at(key);
        } catch (std::out_of_range& e) {
            return std::nullopt;
        }
    }
    [[nodiscard]] std::optional<std::reference_wrapper<pmtv::pmt>>
    get(const std::string& key) noexcept
    {
        try { // TODO: missing find function in pmtv::map -> exception based look-up
            return _map.at(key);
        } catch (std::out_of_range&) {
            return std::nullopt;
        }
    }

    void insert_or_assign(const std::pair<std::string, pmtv::pmt>& value)
    {
        // TODO: expose std::map compatible insert & insert_or_assign interface
        _map[value.first] = value.second;
    }
    void insert_or_assign(const std::string& key, const pmtv::pmt& value)
    {
        // TODO: expose std::map compatible insert & insert_or_assign interface
        _map[key] = value;
        // return _map.insert_or_assign(key, std::move(value));
    }

    uint64_t offset() const { return _offset; }
    const tag_map& map() const { return _map; }

    size_t serialize(std::streambuf& sb) const;
    static tag_t deserialize(std::streambuf& sb);

private:
    uint64_t _offset = 0;
    tag_map _map;
};

// clang-format off
constexpr const char* GR_TAG_PREFIX = "gr:";

template <typename PMT_TYPE>
class default_tag
{ // TODO: consider replacing with a compile-time constexpr fixed-length string
    const std::string _key;
    const std::string _keyShort;
    std::string _unit;
    const std::string _description;

public:
    using value_type = PMT_TYPE;
    default_tag(const std::string_view& key,
                const std::string_view& unit = "",
                const std::string_view& description = "")
        : _key(std::string{ GR_TAG_PREFIX }.append(key)),
          _keyShort(key),
          _unit(unit),
          _description(description)
    {
    }
    default_tag() = delete;

    [[nodiscard]] const std::string& key() const noexcept { return _key; }
    [[nodiscard]] const std::string& shortKey() const noexcept { return _keyShort; }
    [[nodiscard]] const std::string& unit() const noexcept { return _unit; }
    [[nodiscard]] const std::string& description() const noexcept { return _description; }

    [[nodiscard]] operator const std::string&() const noexcept { return _key; }

    template <typename T>
        requires std::is_same_v<value_type, T>
    [[nodiscard]] std::pair<std::string, pmtv::pmt>
    operator()(const T& newValue) const noexcept
    {
        return { _key, static_cast<pmtv::pmt>(PMT_TYPE(newValue)) };
    }
};

namespace tag { // definition of default tags and names
//TODO: change 'inline static const' to 'inline constexpr' once pmtv supports constexpr
inline static const default_tag<float>    SAMPLE_RATE    = { "sample_rate", "Hz", "signal sample rate"};
inline static const default_tag<float>    SIGNAL_RATE    = SAMPLE_RATE;
inline static const default_tag<std::string>           SIGNAL_NAME    = { "signal_name", "", "signal name"};
inline static const default_tag<std::string>           SIGNAL_UNIT    = { "signal_unit", "", "signal's physical SI unit"};
inline static const default_tag<float>    SIGNAL_MIN     = { "signal_min", "a.u.", "signal physical max. (e.g. DAQ) limit"};
inline static const default_tag<float>    SIGNAL_MAX     = { "signal_max", "a.u.", "signal physical max. (e.g. DAQ) limit"};
inline static const default_tag<std::string>           TRIGGER_NAME   = { "trigger_name"};
inline static const default_tag<uint64_t> TRIGGER_TIME   = { "trigger_time", "ns", "UTC-based time-stamp"};
inline static const default_tag<float>    TRIGGER_OFFSET = { "trigger_offset", "s", "sample delay w.r.t. the trigger (e.g.compensating analog group delays)"};

} // namespace gr::tag
// clang-format on

} // namespace gr
