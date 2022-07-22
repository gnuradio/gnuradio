#include <gnuradio/logger.h>

/**
 * @brief GR Logging Macros and convenience functions
 *
 */
namespace gr {


logger_config::logger_config(YAML::Node config)
{
    _config = config;
    id = config["id"].as<std::string>();
    pattern = config["pattern"].as<std::string>();

    {
        auto str = config["type"].as<std::string>();
        auto it = logger_type_t_table.find(str);
        if (it != logger_type_t_table.end()) {
            type = it->second;
        }
        else {
            type = logger_type_t::none;
        }
    }

    {
        auto str = config["level"].as<std::string>();
        auto it = logging_level_t_table.find(str);
        if (it != logging_level_t_table.end()) {
            level = it->second;
        }
        else {
            level = logging_level_t::off;
        }
    }
}

// unnecessary
std::shared_ptr<logger_config> logger_config::parse(YAML::Node config)
{
    return std::make_shared<logger_config>(config);
}


logger_console_config::logger_console_config(YAML::Node config) : logger_config(config)
{
    // type = logger_console_type_table[config["console_type"].as<std::string>()];
    auto str = config["console_type"].as<std::string>();
    auto it = logger_console_type_table.find(str);
    if (it != logger_console_type_table.end()) {
        console_type = it->second;
    }
}

logger_sptr logger_console_config::make(const std::string& name,
                                        std::shared_ptr<logger_config> logger_config)
{
    auto cfg = logger_console_config(logger_config->_config);

    if (cfg.console_type == logger_console_type::stdout) {
        return spdlog::stdout_color_mt(name);
    }
    else {
        return spdlog::stderr_color_mt(name);
    }
}


logger_basic_config::logger_basic_config(YAML::Node config) : logger_config(config)
{
    filename = config["filename"].as<std::string>();
}

logger_sptr logger_basic_config::make(const std::string& name,
                                      std::shared_ptr<logger_config> logger_config)
{
    auto cfg = logger_basic_config(logger_config->_config);

    return spdlog::basic_logger_mt(name, cfg.filename);
}


logging_config::logging_config() { parse_from_prefs(); }
void logging_config::parse_from_prefs()
{
    auto node = prefs::get_section("logging");
    for (auto info : node) {
        loggers.push_back(logger_config::parse(info));
    }
}


logger_sptr logging::get_logger(const std::string& logger_name,
                                const std::string& config_name)
{

    // Use the spdlog global registry
    logger_sptr requested_logger = spdlog::get(logger_name);
    if (requested_logger) {
        return requested_logger;
    }


    // Find the configuration for this named logger
    logging_config cfg;
    auto it = std::find_if(
        cfg.loggers.begin(), cfg.loggers.end(), [&](std::shared_ptr<logger_config> lg) {
            return lg->id == config_name;
        });

    // Found the configuration, now create the logger
    if (it != cfg.loggers.end()) {
        switch ((*it)->type) {
        case logger_type_t::basic:
            requested_logger = logger_basic_config::make(logger_name, *it);
            break;
        case logger_type_t::console:
            requested_logger = logger_console_config::make(logger_name, *it);
            break;
        default:
            break;
        }
        if (requested_logger) {
            requested_logger->set_level((*it)->level);
            requested_logger->set_pattern((*it)->pattern);
        }
    }
    else {
        // std::cout << "Logger: " << config_name << " not found in configuration"
        //           << std::endl;
        // If a logger is not found, all the loggers will have nullptr.  This printout is
        // annoying in CI tests
        // TODO: have a default logger in the build directory
    }


    return requested_logger;
}


} // namespace gr
