/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_THREAD_BODY_WRAPPER_H
#define INCLUDED_THREAD_BODY_WRAPPER_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/logger.h>
#include <exception>
#include <iostream>

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

    void operator()()
    {
        mask_signals();

        if (d_catch_exceptions) {
            try {
                d_f();
            } catch (boost::thread_interrupted const&) {
            } catch (std::exception const& e) {
                std::ostringstream msg;
                msg << "ERROR thread[" << d_name << "]: " << e.what() << std::endl;
                GR_LOG_ERROR(d_debug_logger, msg.str());
            } catch (...) {
                std::ostringstream msg;
                msg << "ERROR thread[" << d_name << "]: "
                    << "caught unrecognized exception" << std::endl;
                GR_LOG_ERROR(d_debug_logger, msg.str());
            }

        } else {
            try {
                d_f();
            } catch (boost::thread_interrupted const&) {
            }
        }
    }
};

} /* namespace thread */
} /* namespace gr */

#endif /* INCLUDED_THREAD_BODY_WRAPPER_H */
