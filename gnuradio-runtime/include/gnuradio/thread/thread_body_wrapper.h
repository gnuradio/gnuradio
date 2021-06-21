/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_THREAD_BODY_WRAPPER_H
#define INCLUDED_THREAD_BODY_WRAPPER_H

#include <gnuradio/api.h>
#include <gnuradio/basic_error_handler.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <exception>

namespace gr {
namespace thread {

GR_RUNTIME_API void mask_signals();

template <class F>
class thread_body_wrapper
{
private:
    F d_f;
    std::string d_name;
    bool d_catch_exceptions;
    static std::shared_ptr<basic_error_handler> errorHandler;
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

public:
    explicit thread_body_wrapper(F f,
                                 const std::string& name = "",
                                 bool catch_exceptions = true)
        : d_f(f), d_name(name), d_catch_exceptions(catch_exceptions)
    {
        gr::configure_default_loggers(d_logger, d_debug_logger, "thread_body_wrapper");
    }

    static void register_error_handler(std::shared_ptr<basic_error_handler> handler)
    {
        errorHandler = handler;
    }

    void operator()()
    {
        mask_signals();

        if (d_catch_exceptions) {
            try {
                d_f();
            } catch (boost::thread_interrupted const&) {
                if (errorHandler) {
                    errorHandler->addError("thread interrupted!");
                }
            } catch (std::exception const& e) {
                std::ostringstream msg;
                msg << "ERROR thread[" << d_name << "]: " << e.what();
                if (errorHandler) {
                    errorHandler->addError(msg.str());
                }
                GR_LOG_ERROR(d_logger, msg.str());
            } catch (...) {
                std::ostringstream msg;
                msg << "ERROR thread[" << d_name << "]: caught unrecognized exception";
                if (errorHandler) {
                    errorHandler->addError(msg.str());
                }
                GR_LOG_ERROR(d_logger, msg.str());
            }

        } else {
            try {
                d_f();
            } catch (boost::thread_interrupted const&) {
                if (errorHandler) {
                    std::ostringstream msg;
                    msg << "ERROR thread[" << d_name << "]: thread interrupted exception";
                    errorHandler->addError(msg.str());
                }
            }
        }
    }
};
template <class F>
std::shared_ptr<basic_error_handler> thread_body_wrapper<F>::errorHandler;
} /* namespace thread */
} /* namespace gr */

#endif /* INCLUDED_THREAD_BODY_WRAPPER_H */
