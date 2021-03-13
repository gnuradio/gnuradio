/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "local_sighandler.h"
#include "vmcircbuf.h"
#include "vmcircbuf_prefs.h"
#include <boost/format.hpp>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

// all the factories we know about
#include "vmcircbuf_createfilemapping.h"
#include "vmcircbuf_mmap_shm_open.h"
#include "vmcircbuf_mmap_tmpfile.h"
#include "vmcircbuf_sysv_shm.h"

gr::thread::mutex s_vm_mutex;

namespace gr {

static const char* FACTORY_PREF_KEY = "vmcircbuf_default_factory";

vmcircbuf::~vmcircbuf() {}

vmcircbuf_factory::~vmcircbuf_factory() {}

// ----------------------------------------------------------------

static vmcircbuf_factory* s_default_factory = 0;

vmcircbuf_factory* vmcircbuf_sysconfig::get_default_factory()
{
    if (s_default_factory)
        return s_default_factory;

    bool verbose = false;

    std::vector<gr::vmcircbuf_factory*> all = all_factories();

    logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "vmcircbuf_sysconfig");

    char name[1024];
    if (gr::vmcircbuf_prefs::get(FACTORY_PREF_KEY, name, sizeof(name)) >= 0) {
        for (unsigned int i = 0; i < all.size(); i++) {
            if (strncmp(name, all[i]->name(), strlen(all[i]->name())) == 0) {
                s_default_factory = all[i];
                GR_LOG_INFO(debug_logger,
                            boost::format("Using %s") % s_default_factory->name());
                return s_default_factory;
            }
        }
    }

    // either we don't have a default, or the default named is not in our
    // list of factories.  Find the first factory that works.

    GR_LOG_INFO(debug_logger, "finding a working factory...");

    for (unsigned int i = 0; i < all.size(); i++) {
        if (test_factory(all[i], verbose)) {
            set_default_factory(all[i]);
            return s_default_factory;
        }
    }

    // We're screwed!
    GR_LOG_ERROR(logger, "unable to find a working factory!");
    throw std::runtime_error("gr::vmcircbuf_sysconfig");
}

std::vector<vmcircbuf_factory*> vmcircbuf_sysconfig::all_factories()
{
    std::vector<vmcircbuf_factory*> result;

    result.push_back(gr::vmcircbuf_createfilemapping_factory::singleton());
#ifdef TRY_SHM_VMCIRCBUF
    result.push_back(gr::vmcircbuf_sysv_shm_factory::singleton());
    result.push_back(gr::vmcircbuf_mmap_shm_open_factory::singleton());
#endif
    result.push_back(gr::vmcircbuf_mmap_tmpfile_factory::singleton());

    return result;
}

void vmcircbuf_sysconfig::set_default_factory(vmcircbuf_factory* f)
{
    gr::vmcircbuf_prefs::set(FACTORY_PREF_KEY, f->name());
    s_default_factory = f;
}


// ------------------------------------------------------------------------
//		    test code for vmcircbuf factories
// ------------------------------------------------------------------------

static void init_buffer(const vmcircbuf& c, int counter, int size)
{
    unsigned int* p = (unsigned int*)c.pointer_to_first_copy();
    for (unsigned int i = 0; i < size / sizeof(int); i++)
        p[i] = counter + i;
}

static bool check_mapping(
    const vmcircbuf& c, int counter, int size, const char* msg, logger_ptr debug_logger)
{
    bool ok = true;

    GR_LOG_INFO(debug_logger, msg);

    unsigned int* p1 = (unsigned int*)c.pointer_to_first_copy();
    unsigned int* p2 = (unsigned int*)c.pointer_to_second_copy();

    for (unsigned int i = 0; i < size / sizeof(int); i++) {
        if (p1[i] != counter + i) {
            ok = false;
            GR_LOG_INFO(debug_logger,
                        boost::format("p1[%d] == %u, expected %u") % i % p1[i] %
                            (counter + i));
            break;
        }
        if (p2[i] != counter + i) {
            GR_LOG_ERROR(debug_logger,
                         boost::format("p1[%d] == %u, expected %u") % i % p2[i] %
                             (counter + i));
            ok = false;
            break;
        }
    }
    if (ok) {
        GR_LOG_INFO(debug_logger, "mapping OK");
    }
    return ok;
}

static const char* memsize(int size)
{
    static std::string buf;
    if (size >= (1 << 20)) {
        buf = str(boost::format("%dMB") % (size / (1 << 20)));
    } else if (size >= (1 << 10)) {
        buf = str(boost::format("%dKB") % (size / (1 << 10)));
    } else {
        buf = str(boost::format("%d") % size);
    }
    return buf.c_str();
}

static bool
test_a_bunch(vmcircbuf_factory* factory, int n, int size, int* start_ptr, bool verbose)
{
    bool ok = true;
    std::vector<int> counter(n);
    std::vector<std::unique_ptr<vmcircbuf>> c(n);
    int cum_size = 0;

    logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "gr::test_a_bunch");

    for (int i = 0; i < n; i++) {
        counter[i] = *start_ptr;
        *start_ptr += size;
        if ((c[i] = std::unique_ptr<vmcircbuf>(factory->make(size))) == 0) {
            GR_LOG_INFO(debug_logger,
                        boost::format("Failed to allocate gr::vmcircbuf "
                                      "number %d of size %d (cum = %s)") %
                            (i + 1) % size % memsize(cum_size))
            return false;
        }
        init_buffer(*c[i], counter[i], size);
        cum_size += size;
    }

    for (int i = 0; i < n; i++) {
        std::string msg =
            str(boost::format("test_a_bunch_%dx%s[%d]") % n % memsize(size) % i);
        ok = check_mapping(*c[i], counter[i], size, msg.c_str(), debug_logger) && ok;
    }
    return ok;
}

static bool standard_tests(vmcircbuf_factory* f, int verbose)
{
    logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "standard_tests");

    GR_LOG_INFO(debug_logger, boost::format("Testing %s...") % f->name());

    bool v = verbose >= 2;
    int granularity = f->granularity();
    int start = 0;
    bool ok = true;

    ok = test_a_bunch(f, 1, 1 * granularity, &start, v) && ok; //   1 x   4KB =   4KB

    if (ok) {
        ok = test_a_bunch(f, 64, 4 * granularity, &start, v) && ok; //  64 x  16KB =   1MB
        ok = test_a_bunch(f, 4, 4 * (1L << 20), &start, v) && ok;   //   4 x   4MB =  16MB
        //  ok = test_a_bunch(f, 256, 256 * (1L << 10),  &start, v) && ok;  // 256 x 256KB
        //  = 64MB
    }

    GR_LOG_INFO(debug_logger,
                boost::format("%s: %s") % f->name() % (ok ? "OK" : "Doesn't work"))
    return ok;
}

bool vmcircbuf_sysconfig::test_factory(vmcircbuf_factory* f, int verbose)
{
    logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "gr::vmcircbuf_sysconfig");
    // Install local signal handlers for SIGSEGV and SIGBUS.
    // If something goes wrong, these signals may be invoked.

#ifdef SIGSEGV
    gr::local_sighandler sigsegv(SIGSEGV, gr::local_sighandler::throw_signal);
#endif
#ifdef SIGBUS
    gr::local_sighandler sigbus(SIGBUS, gr::local_sighandler::throw_signal);
#endif
#ifdef SIGSYS
    gr::local_sighandler sigsys(SIGSYS, gr::local_sighandler::throw_signal);
#endif

    try {
        return standard_tests(f, verbose);
    } catch (gr::signal& sig) {
        GR_LOG_INFO(debug_logger,
                    boost::format("vmcircbuf_factory::test_factory (%s): caught %s") %
                        f->name() % sig.name().c_str())
        return false;
    } catch (...) {
        GR_LOG_WARN(debug_logger,
                    boost::format("vmcircbuf_factory::test_factory (%s) some "
                                  "kind of uncaught exception.") %
                        f->name())
        return false;
    }
    return false; // never gets here.  shut compiler up.
}

bool vmcircbuf_sysconfig::test_all_factories(int verbose)
{
    bool ok = false;

    std::vector<vmcircbuf_factory*> all = all_factories();

    for (unsigned int i = 0; i < all.size(); i++)
        ok |= test_factory(all[i], verbose);

    return ok;
}

} /* namespace gr */
