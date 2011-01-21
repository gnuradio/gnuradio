/* -*- c++ -*- */
/*
 * Copyright 2002,2008,2009,2011 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gr_fir_sysconfig_armv7_a.h>
#include <gr_cpu.h>

#include <gr_fir_ccf.h>
#include <gr_fir_ccf_generic.h>
#include <gr_fir_ccf_armv7_a.h>
#include <gr_fir_fcc.h>
#include <gr_fir_fcc_generic.h>
#include <gr_fir_fff.h>
#include <gr_fir_fff_generic.h>
#include <gr_fir_fff_armv7_a.h>
#include <gr_fir_fsf.h>
#include <gr_fir_fsf_generic.h>
#include <gr_fir_ccc.h>
#include <gr_fir_ccc_generic.h>
#include <gr_fir_scc.h>
#include <gr_fir_scc_generic.h>

#include <iostream>
using std::cerr;

///\todo Remove commented out code for altivec and replace with NEON versions.

/*
 * ----------------------------------------------------------------
 * static functions that serve as constructors...
 * ----------------------------------------------------------------
 */

static gr_fir_ccf *
make_gr_fir_ccf_armv7_a (const std::vector<float> &taps)
{
  return new gr_fir_ccf_armv7_a(taps);
}

#if 0
static gr_fir_fcc *
make_gr_fir_fcc_altivec(const std::vector<gr_complex> &taps)
{
  return new gr_fir_fcc_altivec(taps);
}

static gr_fir_ccc *
make_gr_fir_ccc_altivec (const std::vector<gr_complex> &taps)
{
  return new gr_fir_ccc_altivec (taps);
}
#endif

static gr_fir_fff *
make_gr_fir_fff_armv7_a (const std::vector<float> &taps)
{
  return new gr_fir_fff_armv7_a (taps);
}

#if 0
static gr_fir_fsf *
make_gr_fir_fsf_altivec (const std::vector<float> &taps)
{
  return new gr_fir_fsf_altivec (taps);
}

static gr_fir_scc *
make_gr_fir_scc_altivec(const std::vector<gr_complex> &taps)
{
  return new gr_fir_scc_altivec(taps);
}
#endif

/*
 * ----------------------------------------------------------------
 * Return instances of the fastest arm versions of these classes.
 *
 * check CPUID, if has armv7-a, return armv7-a version,
 *		else return generic version. This will break 
 *		when someone makes an armv7-a without a NEON
 *		coprocessor.
 * ----------------------------------------------------------------
 */

gr_fir_ccf *
gr_fir_sysconfig_armv7_a::create_gr_fir_ccf (const std::vector<float> &taps)
{
  static bool first = true;

  if (gr_cpu::has_armv7_a ()){
    if (first){
      cerr << ">>> gr_fir_ccf: using armv7_a\n";
      first = false;
    }
    return make_gr_fir_ccf_armv7_a (taps);
  }

  if (0 && first){
    cerr << ">>> gr_fir_ccf: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_ccf (taps);
}

gr_fir_fcc *
gr_fir_sysconfig_armv7_a::create_gr_fir_fcc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

#if 0
  if (gr_cpu::has_altivec ()){
    if (first){
      cerr << ">>> gr_fir_fcc: using altivec\n";
      first = false;
    }
    return make_gr_fir_fcc_altivec (taps);
  }
#endif

  if (0 && first){
    cerr << ">>> gr_fir_fcc: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fcc (taps);
}

gr_fir_ccc *
gr_fir_sysconfig_armv7_a::create_gr_fir_ccc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

#if 0
  if (gr_cpu::has_altivec ()){
    if (first){
      cerr << ">>> gr_fir_ccc: using altivec\n";
      first = false;
    }
    return make_gr_fir_ccc_altivec (taps);
  }
#endif
  
  if (0 && first){
    cerr << ">>> gr_fir_ccc: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_ccc (taps);
}

gr_fir_fff *
gr_fir_sysconfig_armv7_a::create_gr_fir_fff (const std::vector<float> &taps)
{
  static bool first = true;

  if (gr_cpu::has_armv7_a ()){
    if (first){
      cerr << ">>> gr_fir_fff: using armv7_a\n";
      first = false;
    }
    return make_gr_fir_fff_armv7_a (taps);
  }
  
  if (0 && first){
    cerr << ">>> gr_fir_fff: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fff (taps);
}

gr_fir_fsf *
gr_fir_sysconfig_armv7_a::create_gr_fir_fsf (const std::vector<float> &taps)
{
  static bool first = true;

#if 0
  if (gr_cpu::has_altivec ()){
    if (first){
      cerr << ">>> gr_fir_fsf: using altivec\n";
      first = false;
    }
    return make_gr_fir_fsf_altivec (taps);
  }
#endif
  
  if (0 && first){
    cerr << ">>> gr_fir_fsf: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fsf (taps);
}


gr_fir_scc *
gr_fir_sysconfig_armv7_a::create_gr_fir_scc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

#if 0
  if (gr_cpu::has_altivec ()){
    if (first){
      cerr << ">>> gr_fir_scc: using altivec\n";
      first = false;
    }
    return make_gr_fir_scc_altivec (taps);
  }
#endif

  if (0 && first){
    cerr << ">>> gr_fir_scc: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_scc (taps);
}

/*
 * ----------------------------------------------------------------
 *         Return info about available implementations
 * ----------------------------------------------------------------
 */

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_ccf_info (std::vector<gr_fir_ccf_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_ccf_info (info);

  // add our stuff...
  gr_fir_ccf_info	t;
  if (gr_cpu::has_armv7_a ()){
    t.name = "armv7_a";
    t.create = make_gr_fir_ccf_armv7_a;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_fcc_info (std::vector<gr_fir_fcc_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fcc_info (info);

#if 0
  // add our stuff...
  gr_fir_fcc_info	t;
  if (gr_cpu::has_altivec ()){
    t.name = "altivec";
    t.create = make_gr_fir_fcc_altivec;
    (*info).push_back (t);
  }
#endif
}

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_ccc_info (std::vector<gr_fir_ccc_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_ccc_info (info);

#if 0
  // add our stuff...
  gr_fir_ccc_info	t;
  if (gr_cpu::has_altivec ()){
    t.name = "altivec";
    t.create = make_gr_fir_ccc_altivec;
    (*info).push_back (t);
  }
#endif
}

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_fff_info (std::vector<gr_fir_fff_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fff_info (info);

  // add our stuff...
  gr_fir_fff_info	t;
  if (gr_cpu::has_armv7_a ()){
    t.name = "armv7_a";
    t.create = make_gr_fir_fff_armv7_a;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_fsf_info (std::vector<gr_fir_fsf_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fsf_info (info);

#if 0
  // add our stuff...
  gr_fir_fsf_info	t;
  if (gr_cpu::has_altivec ()){
    t.name = "altivec";
    t.create = make_gr_fir_fsf_altivec;
    (*info).push_back (t);
  }
#endif
}

void 
gr_fir_sysconfig_armv7_a::get_gr_fir_scc_info (std::vector<gr_fir_scc_info> *info)
{
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_scc_info (info);

#if 0
  // add our stuff...
  gr_fir_scc_info	t;
  if (gr_cpu::has_altivec ()){
    t.name = "altivec";
    t.create = make_gr_fir_scc_altivec;
    (*info).push_back (t);
  }
#endif
}
