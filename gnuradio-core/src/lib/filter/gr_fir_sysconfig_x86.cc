/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#include <gr_fir_sysconfig_x86.h>
#include <gr_cpu.h>

#include <gr_fir_ccf.h>
#include <gr_fir_ccf_generic.h>
#include <gr_fir_ccf_x86.h>
#include <gr_fir_fcc.h>
#include <gr_fir_fcc_generic.h>
#include <gr_fir_fcc_x86.h>
#include <gr_fir_fff.h>
#include <gr_fir_fff_generic.h>
#include <gr_fir_fff_x86.h>
#include <gr_fir_fsf.h>
#include <gr_fir_fsf_generic.h>
#include <gr_fir_fsf_x86.h>
#include <gr_fir_ccc.h>
#include <gr_fir_ccc_generic.h>
#include <gr_fir_ccc_x86.h>
#include <gr_fir_scc.h>
#include <gr_fir_scc_generic.h>
#include <gr_fir_scc_x86.h>
// #include <gr_fir_sss.h>
// #include <gr_fir_sss_generic.h>
// #include <gr_fir_sss_mmx.h>
// #include <gr_fir_sss_sse2.h>

#include <iostream>
using std::cerr;

/*
 * ----------------------------------------------------------------
 * static functions that serve as constructors...
 * Is it possible to take the address of a normal constructor?
 * ----------------------------------------------------------------
 */

static gr_fir_ccf *
make_gr_fir_ccf_3dnow(const std::vector<float> &taps)
{
  return new gr_fir_ccf_3dnow(taps);
}

static gr_fir_ccf *
make_gr_fir_ccf_sse(const std::vector<float> &taps)
{
  return new gr_fir_ccf_sse(taps);
}

static gr_fir_fcc *
make_gr_fir_fcc_3dnow(const std::vector<gr_complex> &taps)
{
  return new gr_fir_fcc_3dnow(taps);
}

static gr_fir_fcc *
make_gr_fir_fcc_sse(const std::vector<gr_complex> &taps)
{
  return new gr_fir_fcc_sse(taps);
}

static gr_fir_ccc *
make_gr_fir_ccc_3dnow (const std::vector<gr_complex> &taps)
{
  return new gr_fir_ccc_3dnow (taps);
}

static gr_fir_ccc *
make_gr_fir_ccc_3dnowext (const std::vector<gr_complex> &taps)
{
  return new gr_fir_ccc_3dnowext (taps);
}

static gr_fir_ccc *
make_gr_fir_ccc_sse (const std::vector<gr_complex> &taps)
{
  return new gr_fir_ccc_sse (taps);
}

static gr_fir_fff *
make_gr_fir_fff_3dnow (const std::vector<float> &taps)
{
  return new gr_fir_fff_3dnow (taps);
}

static gr_fir_fff *
make_gr_fir_fff_sse (const std::vector<float> &taps)
{
  return new gr_fir_fff_sse (taps);
}

static gr_fir_fsf *
make_gr_fir_fsf_3dnow (const std::vector<float> &taps)
{
  return new gr_fir_fsf_3dnow (taps);
}

static gr_fir_fsf *
make_gr_fir_fsf_sse (const std::vector<float> &taps)
{
  return new gr_fir_fsf_sse (taps);
}

#if 0
static gr_fir_sss *
make_gr_fir_sss_mmx (const std::vector<short> &taps)
{
  return new gr_fir_sss_mmx (taps);
}

static gr_fir_sss *
make_gr_fir_sss_sse2 (const std::vector<short> &taps)
{
  return new gr_fir_sss_sse2 (taps);
}
#endif

static gr_fir_scc *
make_gr_fir_scc_3dnow(const std::vector<gr_complex> &taps)
{
  return new gr_fir_scc_3dnow(taps);
}

static gr_fir_scc *
make_gr_fir_scc_3dnowext(const std::vector<gr_complex> &taps)
{
  return new gr_fir_scc_3dnowext(taps);
}

static gr_fir_scc *
make_gr_fir_scc_sse(const std::vector<gr_complex> &taps)
{
  return new gr_fir_scc_sse(taps);
}

/*
 * ----------------------------------------------------------------
 * Return instances of the fastest x86 versions of these classes.
 *
 * check CPUID, if has 3DNowExt, return 3DNow!Ext version,
 *              else if 3DNow, return 3DNow! version,
 *              else if SSE2, return SSE2 version,
 *		else if SSE, return SSE version,
 *		else if MMX, return MMX version,
 *		else return generic version.
 *
 * FIXME: benchmark, store result, use stored result to
 *   select the fastest version.
 * ----------------------------------------------------------------
 */

gr_fir_ccf *
gr_fir_sysconfig_x86::create_gr_fir_ccf (const std::vector<float> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnow ()){
    if (first){
      cerr << ">>> gr_fir_ccf: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_ccf_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_ccf: using SSE\n";
      first = false;
    }
    return make_gr_fir_ccf_sse (taps);
  }

  if (first){
    cerr << ">>> gr_fir_ccf: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_ccf (taps);
}

gr_fir_fcc *
gr_fir_sysconfig_x86::create_gr_fir_fcc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnow ()){
    if (first){
      cerr << ">>> gr_fir_fcc: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_fcc_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_fcc: using SSE\n";
      first = false;
    }
    return make_gr_fir_fcc_sse (taps);
  }

  if (first){
    cerr << ">>> gr_fir_fcc: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fcc (taps);
}

gr_fir_ccc *
gr_fir_sysconfig_x86::create_gr_fir_ccc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnowext ()){
    if (first) {
      cerr << ">>> gr_fir_ccc: using 3DNow!Ext\n";
      first = false;
    }
    return make_gr_fir_ccc_3dnowext (taps);
  }

  if (gr_cpu::has_3dnow ()){
    if (first) {
      cerr << ">>> gr_fir_ccc: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_ccc_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_ccc: using SSE\n";
      first = false;
    }
    return make_gr_fir_ccc_sse (taps);
  }
  
  if (first){
    cerr << ">>> gr_fir_ccc: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_ccc (taps);
}

gr_fir_fff *
gr_fir_sysconfig_x86::create_gr_fir_fff (const std::vector<float> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnow ()){
    if (first) {
      cerr << ">>> gr_fir_fff: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_fff_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_fff: using SSE\n";
      first = false;
    }
    return make_gr_fir_fff_sse (taps);
  }
  
  if (first){
    cerr << ">>> gr_fir_fff: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fff (taps);
}

gr_fir_fsf *
gr_fir_sysconfig_x86::create_gr_fir_fsf (const std::vector<float> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnow ()){
    if (first) {
      cerr << ">>> gr_fir_fsf: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_fsf_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_fsf: using SSE\n";
      first = false;
    }
    return make_gr_fir_fsf_sse (taps);
  }
  
  if (first){
    cerr << ">>> gr_fir_fsf: handing off to parent class\n";
    first = false;
  }
  return gr_fir_sysconfig_generic::create_gr_fir_fsf (taps);
}

#if 0
gr_fir_sss *
gr_fir_sysconfig_x86::create_gr_fir_sss (const std::vector<short> &taps)
{
  // FIXME -- probably want to figure out best answer for Athlon and code
  // add code to select it here...

  if (gr_cpu::has_sse2 ()){
    cerr << ">>> gr_fir_sss: using SSE2\n";
    return make_gr_fir_sss_sse2 (taps);
  }
  
  if (gr_cpu::has_mmx ()){
    cerr << ">>> gr_fir_sss: using MMX\n";
    return make_gr_fir_sss_mmx (taps);
  }
  
  cerr << ">>> gr_fir_sss: handing off to parent class\n";
  return gr_fir_sysconfig_generic::create_gr_fir_sss (taps);
}
#endif

gr_fir_scc *
gr_fir_sysconfig_x86::create_gr_fir_scc (const std::vector<gr_complex> &taps)
{
  static bool first = true;

  if (gr_cpu::has_3dnowext ()){
    if (first){
      cerr << ">>> gr_fir_scc: using 3DNow!Ext\n";
      first = false;
    }
    return make_gr_fir_scc_3dnowext (taps);
  }

  if (gr_cpu::has_3dnow ()){
    if (first){
      cerr << ">>> gr_fir_scc: using 3DNow!\n";
      first = false;
    }
    return make_gr_fir_scc_3dnow (taps);
  }

  if (gr_cpu::has_sse ()){
    if (first){
      cerr << ">>> gr_fir_scc: using SSE\n";
      first = false;
    }
    return make_gr_fir_scc_sse (taps);
  }

  if (first){
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
gr_fir_sysconfig_x86::get_gr_fir_ccf_info (std::vector<gr_fir_ccf_info> *info)
{
  gr_fir_ccf_info	t;

  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_ccf_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_ccf_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_ccf_sse;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_x86::get_gr_fir_fcc_info (std::vector<gr_fir_fcc_info> *info)
{
  gr_fir_fcc_info	t;

  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fcc_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_fcc_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_fcc_sse;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_x86::get_gr_fir_ccc_info (std::vector<gr_fir_ccc_info> *info)
{
  gr_fir_ccc_info	t;
  
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_ccc_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnowext ()){
    t.name = "3DNow!Ext";
    t.create = make_gr_fir_ccc_3dnowext;
    (*info).push_back (t);
  }

  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_ccc_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_ccc_sse;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_x86::get_gr_fir_fff_info (std::vector<gr_fir_fff_info> *info)
{
  gr_fir_fff_info	t;
  
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fff_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_fff_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_fff_sse;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_x86::get_gr_fir_fsf_info (std::vector<gr_fir_fsf_info> *info)
{
  gr_fir_fsf_info	t;
  
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_fsf_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_fsf_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_fsf_sse;
    (*info).push_back (t);
  }
}

void 
gr_fir_sysconfig_x86::get_gr_fir_scc_info (std::vector<gr_fir_scc_info> *info)
{
  gr_fir_scc_info	t;

  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_scc_info (info);

  // add our stuff...
  if (gr_cpu::has_3dnowext ()){
    t.name = "3DNow!Ext";
    t.create = make_gr_fir_scc_3dnowext;
    (*info).push_back (t);
  }

  if (gr_cpu::has_3dnow ()){
    t.name = "3DNow!";
    t.create = make_gr_fir_scc_3dnow;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse ()){
    t.name = "SSE";
    t.create = make_gr_fir_scc_sse;
    (*info).push_back (t);
  }
}

#if 0
void 
gr_fir_sysconfig_x86::get_gr_fir_sss_info (std::vector<gr_fir_sss_info> *info)
{
  gr_fir_sss_info	t;
  
  // invoke parent..
  gr_fir_sysconfig_generic::get_gr_fir_sss_info (info);

  // add our stuff...
  if (gr_cpu::has_mmx ()){
    t.name = "MMX";
    t.create = make_gr_fir_sss_mmx;
    (*info).push_back (t);
  }

  if (gr_cpu::has_sse2 ()){
    t.name = "SSE2";
    t.create = make_gr_fir_sss_sse2;
    (*info).push_back (t);
  }
}
#endif
