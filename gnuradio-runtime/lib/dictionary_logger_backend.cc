#include "gnuradio/dictionary_logger_backend.h"
#include <spdlog/details/log_msg.h>
#include <memory>
#include <mutex>
#include <regex>
#include <tuple>


namespace gr {
dictionary_logger_backend::dictionary_logger_backend(std::regex src_regex)
    : has_regex(true), src_regex(src_regex)
{
}
dictionary_logger_backend::dictionary_logger_backend() {}

void dictionary_logger_backend::sink_it_(const spdlog::details::log_msg& message)
{
    const auto& logger_name = message.logger_name;
    if (has_regex) {
        if (!std::regex_match(logger_name.begin(), logger_name.end(), src_regex)) {
            // do nothing if regex is set and does not match
            return;
        }
    }

    const auto& payload = message.payload;
    auto lock = std::scoped_lock(map_mutex);

    auto& message_set =
        log_entries[std::string{ logger_name.data(), logger_name.size() }];
    // usually, log messages should come in chronologically
    message_set.emplace_hint(message_set.end(),
                             log_entry(message.time, { payload.data(), payload.size() }));
}
// void dictionary_logger_backend::flush_() {}

} // namespace gr
