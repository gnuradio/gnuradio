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
#include <string_view>
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

    gr::logger logger("vmcircbuf_sysconfig");

    const auto name = gr::vmcircbuf_prefs::get(FACTORY_PREF_KEY);
    if (name) {
        for (const auto& factory : all) {
            if (name == factory->name()) {
                s_default_factory = factory;
                logger.info("Using {:s}", s_default_factory->name());
                return s_default_factory;
            }
        }
    }

    // either we don't have a default, or the default named is not in our
    // list of factories.  Find the first factory that works.
    logger.info("No vmcircbuf preference read; this isn't inherently a problem. "
                "Defaulting to the highest-priority working factory...");

    for (const auto& factory : all) {
        if (test_factory(factory, verbose)) {
            logger.info("Using {:s} factory. Trying to save this configuration…",
                        factory->name());
            set_default_factory(factory);
            return s_default_factory;
        }
    }

    // We're screwed!
    logger.error("unable to find a working factory!");
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
    const vmcircbuf& c, int counter, int size, const char* msg, gr::logger& logger)
{
    bool ok = true;

    logger.info("{:s}", msg);

    unsigned int* p1 = (unsigned int*)c.pointer_to_first_copy();
    unsigned int* p2 = (unsigned int*)c.pointer_to_second_copy();

    for (unsigned int i = 0; i < size / sizeof(int); i++) {
        if (p1[i] != counter + i) {
            ok = false;
            logger.error("p1[{:d}] == {:d}, expected {:d}", i, p1[i], counter + i);
            break;
        }
        if (p2[i] != counter + i) {
            logger.error("p2[{:d}] == {:d}, expected {:d}", i, p2[i], counter + i);
            ok = false;
            break;
        }
    }
    if (ok) {
        logger.info("mapping OK");
    }
    return ok;
}

static const char* memsize(int size)
{
    static std::string buf;
    if (size >= (1 << 20)) {
        buf = std::to_string(size / (1 << 20)) + "MB";
    } else if (size >= (1 << 10)) {
        buf = std::to_string(size / (1 << 10)) + "kB";
    } else {
        buf = std::to_string(size);
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

    gr::logger logger("gr::test_a_bunch");

    for (int i = 0; i < n; i++) {
        counter[i] = *start_ptr;
        *start_ptr += size;
        if ((c[i] = std::unique_ptr<vmcircbuf>(factory->make(size))) == 0) {
            logger.info(
                "Failed to allocate gr::vmcircbuf number {:d} of size {:d} (cum = {:s})",
                i + 1,
                size,
                memsize(cum_size));
            return false;
        }
        init_buffer(*c[i], counter[i], size);
        cum_size += size;
    }

    for (int i = 0; i < n; i++) {
        std::string msg = "test_a_bunch_" + std::to_string(n) + "x" + memsize(size) +
                          "[" + std::to_string(i) + "]";
        ok = check_mapping(*c[i], counter[i], size, msg.c_str(), logger) && ok;
    }
    return ok;
}

static bool standard_tests(vmcircbuf_factory* f, int verbose)
{

    gr::logger logger("standard tests");
    logger.info("Testing {:s}...", f->name());

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

    logger.info("{:s}: {:s}", f->name(), ok ? "OK" : "Doesn't work");
    return ok;
}

bool vmcircbuf_sysconfig::test_factory(vmcircbuf_factory* f, int verbose)
{
    gr::logger logger("gr::vmcircbuf_sysconfig");
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
        logger.info(
            "vmcircbuf_factory::test_factory ({:s}): caught {:s}", f->name(), sig.name());
        return false;
    } catch (...) {
        logger.warn(
            "vmcircbuf_factory::test_factory ({:s}) some kind of uncaught exception.",
            f->name());
        return false;
    }
    return false; // never gets here.  shut compiler up.
}

bool vmcircbuf_sysconfig::test_all_factories(int verbose)
{
    std::vector<vmcircbuf_factory*> all = all_factories();
    if (all.empty())
        return false;

    gr::logger log("test all factories");
    bool any_ok = false;
    for (auto& factory : all) {
        bool success = test_factory(factory, verbose);
        log.debug("testing {}: {}", factory->name(), success);
        any_ok |= success;
    }
    return any_ok;
}

} /* namespace gr */
