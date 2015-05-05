/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <boost/format.hpp>
#include "vmcircbuf.h"
#include "vmcircbuf_prefs.h"
#include "local_sighandler.h"

// all the factories we know about
#include "vmcircbuf_createfilemapping.h"
#include "vmcircbuf_sysv_shm.h"
#include "vmcircbuf_mmap_shm_open.h"
#include "vmcircbuf_mmap_tmpfile.h"

gr::thread::mutex s_vm_mutex;

namespace gr {

  static const char *FACTORY_PREF_KEY = "vmcircbuf_default_factory";

  vmcircbuf::~vmcircbuf()
  {
  }

  vmcircbuf_factory::~vmcircbuf_factory()
  {
  }

  // ----------------------------------------------------------------

  static vmcircbuf_factory *s_default_factory = 0;

  vmcircbuf_factory *
  vmcircbuf_sysconfig::get_default_factory()
  {
    if(s_default_factory)
      return s_default_factory;

    bool verbose = false;

    std::vector<gr::vmcircbuf_factory *> all = all_factories ();

    char name[1024];
    if (gr::vmcircbuf_prefs::get(FACTORY_PREF_KEY, name, sizeof(name)) >= 0) {
      for(unsigned int i = 0; i < all.size (); i++) {
        if(strncmp(name, all[i]->name(), strlen(all[i]->name())) == 0) {
          s_default_factory = all[i];
          if(verbose)
            fprintf(stderr, "gr::vmcircbuf_sysconfig: using %s\n",
                    s_default_factory->name());
          return s_default_factory;
        }
      }
    }

    // either we don't have a default, or the default named is not in our
    // list of factories.  Find the first factory that works.

    if(verbose)
      fprintf(stderr, "gr::vmcircbuf_sysconfig: finding a working factory...\n");

    for(unsigned int i = 0; i < all.size (); i++) {
      if(test_factory(all[i], verbose)) {
        set_default_factory(all[i]);
        return s_default_factory;
      }
    }

    // We're screwed!
    fprintf(stderr, "gr::vmcircbuf_sysconfig: unable to find a working factory!\n");
    throw std::runtime_error("gr::vmcircbuf_sysconfig");
  }

  std::vector<vmcircbuf_factory *>
  vmcircbuf_sysconfig::all_factories()
  {
    std::vector<vmcircbuf_factory*> result;

    result.push_back(gr::vmcircbuf_createfilemapping_factory::singleton());
#ifdef TRY_SHM_VMCIRCBUF
    result.push_back(gr::vmcircbuf_sysv_shm_factory::singleton());
    result.push_back(gr::vmcircbuf_mmap_shm_open_factory::singleton());
#endif
    result.push_back (gr::vmcircbuf_mmap_tmpfile_factory::singleton());

    return result;
  }

  void
  vmcircbuf_sysconfig::set_default_factory(vmcircbuf_factory *f)
  {
    gr::vmcircbuf_prefs::set(FACTORY_PREF_KEY, f->name());
    s_default_factory = f;
  }


  // ------------------------------------------------------------------------
  //		    test code for vmcircbuf factories
  // ------------------------------------------------------------------------

  static void
  init_buffer(vmcircbuf *c, int counter, int size)
  {
    unsigned int *p = (unsigned int*)c->pointer_to_first_copy();
    for(unsigned int i = 0; i < size / sizeof(int); i++)
      p[i] = counter + i;
  }

  static bool
  check_mapping(vmcircbuf *c, int counter, int size, const char *msg, bool verbose)
  {
    bool ok = true;

    if(verbose)
      fprintf(stderr, "... %s", msg);

    unsigned int *p1 = (unsigned int *)c->pointer_to_first_copy();
    unsigned int *p2 = (unsigned int *)c->pointer_to_second_copy();

    // fprintf(stderr, "p1 = %p, p2 = %p\n", p1, p2);

    for(unsigned int i = 0; i < size / sizeof (int); i++) {
      if(p1[i] != counter + i) {
        ok = false;
        if(verbose)
          fprintf(stderr, "  p1[%d] == %u, expected %u\n", i, p1[i], counter + i);
        break;
      }
      if(p2[i] != counter + i) {
        if(verbose)
          fprintf(stderr, "  p2[%d] == %u, expected %u\n", i, p2[i], counter + i);
        ok = false;
        break;
      }
    }

    if(ok && verbose) {
      fprintf(stderr, "  OK\n");
    }
    return ok;
  }

  static const char *
  memsize(int size)
  {
    static std::string buf;
    if(size >= (1 << 20)) {
      buf = str(boost::format("%dMB") % (size / (1 << 20)));
    }
    else if(size >= (1 << 10)){
      buf = str(boost::format("%dKB") % (size / (1 << 10)));
    }
    else {
      buf = str(boost::format("%d") % size);
    }
    return buf.c_str();
  }

  static bool
  test_a_bunch(vmcircbuf_factory *factory, int n, int size, int *start_ptr, bool verbose)
  {
    bool ok = true;
    std::vector<int> counter(n);
    std::vector<vmcircbuf*> c(n);
    int cum_size = 0;

    for(int i = 0; i < n; i++) {
      counter[i] = *start_ptr;
      *start_ptr += size;
      if((c[i] = factory->make (size)) == 0) {
        if(verbose)
          fprintf(stderr,
                  "Failed to allocate gr::vmcircbuf number %d of size %d (cum = %s)\n",
                  i + 1, size, memsize(cum_size));
        return false;
      }
      init_buffer(c[i], counter[i], size);
      cum_size += size;
    }

    for(int i = 0; i < n; i++) {
      std::string msg = str(boost::format("test_a_bunch_%dx%s[%d]") % n % memsize(size) % i);
      ok &= check_mapping(c[i], counter[i], size, msg.c_str(), verbose);
    }

    for(int i = 0; i < n; i++) {
      delete c[i];
      c[i] = 0;
    }

    return ok;
  }

  static bool
  standard_tests(vmcircbuf_factory *f, int verbose)
  {
    if(verbose >= 1)
      fprintf(stderr, "Testing %s...\n", f->name());

    bool v = verbose >= 2;
    int granularity = f->granularity();
    int start = 0;
    bool ok = true;

    ok &= test_a_bunch(f,   1,   1 * granularity, &start,  v);   //   1 x   4KB =   4KB

    if(ok) {
      ok &= test_a_bunch(f,  64,   4 * granularity, &start, v);  //  64 x  16KB =   1MB
      ok &= test_a_bunch(f,   4,   4 * (1L << 20),  &start, v);  //   4 x   4MB =  16MB
      //  ok &= test_a_bunch(f, 256, 256 * (1L << 10),  &start, v);  // 256 x 256KB =  64MB
    }

    if(verbose >= 1)
      fprintf(stderr, "....... %s: %s", f->name(), ok ? "OK\n" : "Doesn't work\n");

    return ok;
  }

  bool
  vmcircbuf_sysconfig::test_factory(vmcircbuf_factory *f, int verbose)
  {
    // Install local signal handlers for SIGSEGV and SIGBUS.
    // If something goes wrong, these signals may be invoked.

#ifdef SIGSEGV
    gr::local_sighandler sigsegv (SIGSEGV, gr::local_sighandler::throw_signal);
#endif
#ifdef SIGBUS
    gr::local_sighandler sigbus (SIGBUS, gr::local_sighandler::throw_signal);
#endif
#ifdef SIGSYS
    gr::local_sighandler sigsys (SIGSYS, gr::local_sighandler::throw_signal);
#endif

    try {
      return standard_tests (f, verbose);
    }
    catch(gr::signal &sig) {
      if(verbose) {
        fprintf(stderr, "....... %s: %s", f->name(), "Doesn't work\n");
        fprintf(stderr,
                "gr::vmcircbuf_factory::test_factory (%s): caught %s\n",
                f->name(), sig.name().c_str());
        return false;
      }
    }
    catch (...) {
      if(verbose) {
        fprintf(stderr, "....... %s: %s", f->name(), "Doesn't work\n");
        fprintf(stderr,
                "gr::vmcircbuf_factory::test_factory (%s): some kind of uncaught exception\n",
                f->name());
      }
      return false;
    }
    return false;    // never gets here.  shut compiler up.
  }

  bool
  vmcircbuf_sysconfig::test_all_factories(int verbose)
  {
    bool ok = false;

    std::vector<vmcircbuf_factory *> all = all_factories();

    for(unsigned int i = 0; i < all.size (); i++)
      ok |= test_factory(all[i], verbose);

    return ok;
  }

} /* namespace gr */
