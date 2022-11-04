#pragma once

#include <gnuradio/prefs.h>
#include <yaml-cpp/yaml.h>
#include <string>

namespace gr {

struct scheduler_options {
    std::string name;
    size_t default_buffer_size = 32768;
    std::string default_buffer_type = "cpu_vmcirc";
    bool flush = true;
    size_t flush_count = 8;
    size_t flush_sleep_ms = 10;


    scheduler_options(const std::string& name_) : name(name_) {}
    virtual ~scheduler_options() = default;
    using sptr = std::shared_ptr<scheduler_options>;
    static sptr opts_from_yaml(const std::string& options = "{}")
    {
        auto opt_yaml = YAML::Load(options);
        auto opts = std::make_shared<scheduler_options>(
            opt_yaml["name"].as<std::string>("default"));

        auto prefs_section = fmt::format("scheduler.{}", opts->name);
        opts->default_buffer_size = opt_yaml["buffer_size"].as<size_t>(
            gr::prefs::get_long(prefs_section, "default_buffer_size", 32768));
        opts->default_buffer_type = opt_yaml["default_buffer_type"].as<std::string>(
            gr::prefs::get_string(prefs_section, "default_buffer_type", "cpu_vmcirc"));
        opts->flush =
            opt_yaml["flush"].as<bool>(gr::prefs::get_bool(prefs_section, "flush", true));
        opts->flush_count = opt_yaml["flush_count"].as<long>(
            gr::prefs::get_long(prefs_section, "flush_count", 8));
        opts->flush_sleep_ms = opt_yaml["flush_count_ms"].as<long>(
            gr::prefs::get_long(prefs_section, "flush_count_ms", 10));

        return opts;
    }
};

using scheduler_options_sptr = scheduler_options::sptr;

} // namespace gr
