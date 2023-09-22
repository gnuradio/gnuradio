/* -*- c++ -*- */
/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pmt/pmt.h>
#include <boost/thread.hpp>
#include <exception>
#include <iostream>
#include <regex>
#include <stdexcept>

#ifdef HAVE_LIBUNWIND
#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>
#include <cstdio>
#include <cstdlib>
#endif

namespace gr {

#ifdef HAVE_LIBUNWIND
void print_backtrace()
{
    unw_cursor_t cursor;
    unw_context_t context;
    std::ios_base::fmtflags saved_cerr_flags(std::cerr.flags());

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
            break;
        }
        std::cerr << "0x" << std::hex << pc << ":";

        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            char* nameptr = sym;
            int status;
            char* demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
            if (status == 0) {
                nameptr = demangled;
            }
            std::cerr << " (" << nameptr << "+0x" << std::hex << offset << ")"
                      << std::endl;
            std::free(demangled);
        } else {
            std::cerr << " -- error: unable to obtain symbol name for this frame"
                      << std::endl;
        }
    }

    std::cerr.flags(saved_cerr_flags);
}
#endif

void terminate_handler_impl()
{
    std::cerr << "terminate reached from thread id: " << boost::this_thread::get_id();
    try {
        std::exception_ptr eptr = std::current_exception();
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "Got std::out_of_range" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::length_error& e) {
        std::cerr << "Got std::length_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::domain_error& e) {
        std::cerr << "Got std::domain_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Got std::invalid_argument" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const pmt::exception& e) {
        std::cerr << "Got pmt::exception" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::logic_error& e) {
        std::cerr << "Got std::logic_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Got std::ios_base::failure" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::system_error& e) {
        std::cerr << "Got std::system_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::regex_error& e) {
        std::cerr << "Got std::regex_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::underflow_error& e) {
        std::cerr << "Got std::underflow_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::overflow_error& e) {
        std::cerr << "Got std::overflow_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::range_error& e) {
        std::cerr << "Got std::range_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Got std::runtime_error" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_typeid& e) {
        std::cerr << "Got std::bad_typeid" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_cast& e) {
        std::cerr << "Got std::bad_cast" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_weak_ptr& e) {
        std::cerr << "Got std::bad_weak_ptr" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_function_call& e) {
        std::cerr << "Got std::bad_function_call" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_array_new_length& e) {
        std::cerr << "Got std::bad_array_new_length" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Got std::bad_alloc" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::bad_exception& e) {
        std::cerr << "Got std::bad_exception" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Got std::exception" << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Got unknown exception" << std::endl;
    }
#ifdef HAVE_LIBUNWIND
    // Use libunwind to print a backtrace
    print_backtrace();
#endif
    std::abort();
}

void install_terminate_handler() { std::set_terminate(&terminate_handler_impl); }
} // namespace gr
