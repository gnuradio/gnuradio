/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

/*!
 * \ingroup logging
 * \brief GNU Radio logging wrapper
 *
 */

// Since this file is included in *all* gr::blocks, please make sure this list of includes
// keeps as short as possible; if anything is needed only by the implementation in
// buffer.cc, then only include it there
#include <gnuradio/api.h>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <memory>

#include <spdlog/spdlog.h>

#include <spdlog/sinks/dist_sink.h>

namespace gr {
using log_level = spdlog::level::level_enum;

class GR_RUNTIME_API logging
{
public:
    logging(logging const&) = delete; // delete copy ctor, this is a singleton class
    void operator=(logging const&) = delete; // can't assign to singleton class
    static logging& singleton();             //! \brief get the singleton

    //! \brief get the default logging level
    inline log_level default_level() const { return _default_level; }

    //! \brief get the debug logging level
    inline log_level debug_level() const { return _debug_level; }
    spdlog::sink_ptr default_backend() const;
    //! \brief adds a logging sink
    void add_default_sink(const spdlog::sink_ptr& sink);
    //! \brief adds a debugging sink
    void add_debug_sink(const spdlog::sink_ptr& sink);
    //! \brief add a default-constructed console sink to the default logger
    void add_default_console_sink();
    //! \brief add a default-constructed console sink to the debugging logger
    void add_debug_console_sink();

    static constexpr const char* default_pattern = "%n :%l: %v";

private:
    logging();
    const log_level _default_level, _debug_level;
    std::shared_ptr<spdlog::sinks::dist_sink_mt> _default_backend, _debug_backend;
};

/*!
 * \brief GR_LOG macros
 * \ingroup logging
 *
 * These macros wrap the standard LOG4CPP_LEVEL macros.  The available macros
 * are:
 *  LOG_DEBUG
 *  LOG_INFO
 *  LOG_WARN
 *  LOG_TRACE
 *  LOG_ERROR
 *  LOG_ALERT
 *  LOG_CRIT
 *  LOG_FATAL
 *  LOG_EMERG
 */

/********************* Start  Classes and Methods for Python ******************/
/*!
 * \brief Logger class for referencing loggers in python.  Not
 * needed in C++ (use macros) Wraps and manipulates loggers for
 * python as python has no macros
 * \ingroup logging
 *
 */
class GR_RUNTIME_API logger
{
private:
    /*! \brief pointer to logger associated with this wrapper class */
    std::string _name;
    using underlying_logger_ptr = std::shared_ptr<spdlog::logger>;

public:
    /*!
     * \brief constructor Provide name of logger to associate with this class
     * \param logger_name Name of logger associated with class
     */
    logger(const std::string& logger_name);

    /*! \brief Destructor */
    // FIXME implement or = default
    ~logger() = default;

    underlying_logger_ptr d_logger;

    // Wrappers for logging macros
    /*! \brief inline function, wrapper to set the logger level */
    void set_level(const std::string& level);
    void set_level(const log_level level);

    /*! \brief inline function, wrapper to get the logger level */
    void get_level(std::string& level) const;
    const std::string get_string_level() const;
    log_level get_level() const;

    const std::string& name() const;
    void set_name(const std::string& name);

    /*! \brief inline function, wrapper for TRACE message */
    template <typename... Args>
    inline void trace(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->trace(msg, args...);
    }

    /*! \brief inline function, wrapper for DEBUG message */
    template <typename... Args>
    inline void debug(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->debug(msg, args...);
    }

    /*! \brief inline function, wrapper for INFO message */
    template <typename... Args>
    inline void info(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->info(msg, args...);
    }

    /*! \brief inline function, wrapper for INFO message, DEPRECATED */
    template <typename... Args>
    inline void notice(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->info(msg, args...);
    }

    /*! \brief inline function, wrapper for WARN message */
    template <typename... Args>
    inline void warn(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->warn(msg, args...);
    }

    /*! \brief inline function, wrapper for ERROR message */
    template <typename... Args>
    inline void error(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->error(msg, args...);
    }

    /*! \brief inline function, wrapper for CRITICAL message */
    template <typename... Args>
    inline void crit(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->critical(msg, args...);
    }

    /*! \brief inline function, wrapper for CRITICAL message, DEPRECATED */
    template <typename... Args>
    inline void alert(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->critical(msg, args...);
    }

    /*! \brief inline function, wrapper for CRITICAL message, DEPRECATED */
    template <typename... Args>
    inline void fatal(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->critical(msg, args...);
    }

    /*! \brief inline function, wrapper for CRITICAL message, DEPRECATED */
    template <typename... Args>
    inline void emerg(const spdlog::string_view_t& msg, const Args&... args)
    {
        d_logger->critical(msg, args...);
    }
};
using logger_ptr = std::shared_ptr<logger>;

/*!
 * Function to use the GR prefs files to get and setup the two
 * default loggers defined there. The loggers are unique to the
 * class in which they are called, and we pass it the \p name to
 * identify where the log message originates from. For a GNU Radio
 * block, we use 'alias()' for this value, and this is set up for us
 * automatically in gr::block.
 */
GR_RUNTIME_API bool
configure_default_loggers(gr::logger_ptr& l, gr::logger_ptr& d, const std::string& name);

} /* namespace gr */

// global logging shorthands

#define GR_LOG_TRACE(log, msg)     \
    {                              \
        log->d_logger->trace(msg); \
    }

#define GR_LOG_DEBUG(log, msg)     \
    {                              \
        log->d_logger->debug(msg); \
    }

#define GR_LOG_INFO(log, msg)     \
    {                             \
        log->d_logger->info(msg); \
    }

#define GR_LOG_NOTICE(log, msg)   \
    {                             \
        log->d_logger->info(msg); \
    }


#define GR_LOG_WARN(log, msg)     \
    {                             \
        log->d_logger->warn(msg); \
    }

#define GR_LOG_ERROR(log, msg)     \
    {                              \
        log->d_logger->error(msg); \
    }

#define GR_LOG_CRIT(log, msg)         \
    {                                 \
        log->d_logger->critical(msg); \
    }

#define GR_LOG_ALERT(log, msg)        \
    {                                 \
        log->d_logger->critical(msg); \
    }

#define GR_LOG_FATAL(log, msg)        \
    {                                 \
        log->d_logger->critical(msg); \
    }

#define GR_LOG_EMERG(log, msg)        \
    {                                 \
        log->d_logger->critical(msg); \
    }
