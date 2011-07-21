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

#ifndef INCLUDED_GR_FIR_SCC_X86_H
#define INCLUDED_GR_FIR_SCC_X86_H

#include <gr_core_api.h>
#include <gr_fir_scc_simd.h>

/*!
 * \brief 3DNow! version of gr_fir_scc
 */
class GR_CORE_API gr_fir_scc_3dnow : public gr_fir_scc_simd
{
public:
  gr_fir_scc_3dnow ();
  gr_fir_scc_3dnow (const std::vector<gr_complex> &taps);
};

/*!
 * \brief 3DNow! Ext version of gr_fir_scc
 */
class GR_CORE_API gr_fir_scc_3dnowext : public gr_fir_scc_simd
{
public:
  gr_fir_scc_3dnowext ();
  gr_fir_scc_3dnowext (const std::vector<gr_complex> &taps);
};

/*!
 * \brief SSE version of gr_fir_scc
 */
class GR_CORE_API gr_fir_scc_sse : public gr_fir_scc_simd
{
public:
  gr_fir_scc_sse ();
  gr_fir_scc_sse (const std::vector<gr_complex> &taps);
};

#endif
