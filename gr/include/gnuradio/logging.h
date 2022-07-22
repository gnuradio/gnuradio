#pragma once

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <gnuradio/api.h>
#include <gnuradio/prefs.h>
#include <map>

/**
 * @brief GR Logging Macros and convenience functions
 *
 */
namespace gr {

using logger_sptr = std::shared_ptr<spdlog::logger>;

using logging_level_t = spdlog::level::level_enum;

enum class GR_RUNTIME_API logger_type_t { console, basic, rotating, daily, none };
static std::unordered_map<std::string, logger_type_t> const logger_type_t_table = {
    { "console", logger_type_t::console },
    { "basic", logger_type_t::basic },
    { "rotating", logger_type_t::rotating },
    { "daily", logger_type_t::daily },
    { "none", logger_type_t::none }
};

static std::unordered_map<std::string, logging_level_t> const logging_level_t_table = {
    { "trace", logging_level_t::trace }, { "debug", logging_level_t::debug },
    { "info", logging_level_t::info },   { "warn", logging_level_t::warn },
    { "err", logging_level_t::err },     { "critical", logging_level_t::critical },
    { "off", logging_level_t::off }
};

enum class logger_console_type { stdout, stderr };
static std::unordered_map<std::string, logger_console_type> const
    logger_console_type_table = { { "stdout", logger_console_type::stdout },
                                  { "stderr", logger_console_type::stderr } };


struct GR_RUNTIME_API logger_config {
    YAML::Node _config;

    std::string id;
    logger_type_t type;
    logging_level_t level;
    std::string pattern;

    logger_config(YAML::Node config);

    // unnecessary
    static std::shared_ptr<logger_config> parse(YAML::Node config);
};

struct GR_RUNTIME_API logger_console_config : logger_config {
    logger_console_type console_type;

    logger_console_config(YAML::Node config);

    static logger_sptr make(const std::string& name,
                            std::shared_ptr<logger_config> logger_config);
};

struct GR_RUNTIME_API logger_basic_config : logger_config {
    std::string filename;

    logger_basic_config(YAML::Node config);

    static logger_sptr make(const std::string& name,
                            std::shared_ptr<logger_config> logger_config);
};

class GR_RUNTIME_API logging_config
// follows the structure of the yaml
{
public:
    logging_config();
    std::vector<std::shared_ptr<logger_config>> loggers;

private:
    void parse_from_prefs();
};

class GR_RUNTIME_API logging
{
public:
    static logger_sptr get_logger(const std::string& logger_name,
                                  const std::string& config_name);
};

inline void set_level(logger_sptr logger, logging_level_t log_level)
{
    logger->set_level(log_level);
}
// inline logging_level_t get_level(logger_sptr logger)
// {
//     return logger->get_level();
// }


template <typename... Args>
inline void GR_RUNTIME_API gr_log_debug(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->debug(args...);
    }
}

template <typename... Args>
inline void gr_log_info(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->info(args...);
    }
}

template <typename... Args>
inline void gr_log_trace(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->trace(args...);
    }
}

template <typename... Args>
inline void gr_log_warn(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->warn(args...);
    }
}

template <typename... Args>
inline void gr_log_error(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->error(args...);
    }
}

template <typename... Args>
inline void gr_log_critical(logger_sptr logger, const Args&... args)
{
    if (logger) {
        logger->critical(args...);
    }
}

// Do we need or want these macros if we have inline functions
#define GR_LOG_SET_LEVEL(logger, level) logger->set_level(level);
// #define GR_LOG_GET_LEVEL(logger, level) gr::logger_get_level(logger, level)
#define GR_LOG_DEBUG(logger, ...)          \
    {                                      \
        gr_log_debug(logger, __VA_ARGS__); \
    }

#define GR_LOG_INFO(logger, ...)          \
    {                                     \
        gr_log_info(logger, __VA_ARGS__); \
    }

#define GR_LOG_TRACE(logger, ...)          \
    {                                      \
        gr_log_trace(logger, __VA_ARGS__); \
    }

#define GR_LOG_WARN(logger, ...)          \
    {                                     \
        gr_log_warn(logger, __VA_ARGS__); \
    }

#define GR_LOG_ERROR(logger, ...)          \
    {                                      \
        gr_log_error(logger, __VA_ARGS__); \
    }

#define GR_LOG_CRIT(logger, ...)              \
    {                                         \
        gr_log_critical(logger, __VA_ARGS__); \
    }


#define GR_LOG_ERRORIF(logger, cond, ...)      \
    {                                          \
        if ((cond)) {                          \
            gr_log_error(logger, __VA_ARGS__); \
        }                                      \
    }

#define GR_LOG_ASSERT(logger, cond, ...)          \
    {                                             \
        if (!(cond)) {                            \
            gr_log_critical(logger, __VA_ARGS__); \
            assert(0);                            \
        }                                         \
    }

} // namespace gr