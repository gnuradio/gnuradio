#pragma once

#include <string>

namespace gr {
namespace schedulers {

struct scheduler_nbt_options {
    std::string name = "nbt";
    size_t default_buffer_size = 32768;
    std::string default_buffer_type = "cpu_vmcirc";
    bool flush = true;
    size_t flush_count = 8;
    size_t flush_sleep_ms = 10;
};

} // namespace schedulers
} // namespace gr
