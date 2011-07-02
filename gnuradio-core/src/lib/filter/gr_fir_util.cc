/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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
#include <gr_fir_util.h>
#include <gr_fir_sysconfig.h>

//
// There's no problem that can't be solved by the addition of
// another layer of indirection...
//

// --- constructors ---


gr_fir_ccf *
gr_fir_util::create_gr_fir_ccf (const std::vector<float> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_ccf (taps);
}

gr_fir_fcc *
gr_fir_util::create_gr_fir_fcc (const std::vector<gr_complex> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_fcc (taps);
}

gr_fir_ccc *
gr_fir_util::create_gr_fir_ccc (const std::vector<gr_complex> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_ccc (taps);
}

gr_fir_fff *
gr_fir_util::create_gr_fir_fff (const std::vector<float> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_fff (taps);
}

gr_fir_scc *
gr_fir_util::create_gr_fir_scc (const std::vector<gr_complex> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_scc (taps);
}

gr_fir_fsf *
gr_fir_util::create_gr_fir_fsf (const std::vector<float> &taps)
{
  return gr_fir_sysconfig_singleton()->create_gr_fir_fsf (taps);
}

// --- info gatherers ---


void
gr_fir_util::get_gr_fir_ccf_info (std::vector<gr_fir_ccf_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_ccf_info (info);
}

void
gr_fir_util::get_gr_fir_fcc_info (std::vector<gr_fir_fcc_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_fcc_info (info);
}

void
gr_fir_util::get_gr_fir_ccc_info (std::vector<gr_fir_ccc_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_ccc_info (info);
}

void
gr_fir_util::get_gr_fir_fff_info (std::vector<gr_fir_fff_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_fff_info (info);
}

void
gr_fir_util::get_gr_fir_scc_info (std::vector<gr_fir_scc_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_scc_info (info);
}

void
gr_fir_util::get_gr_fir_fsf_info (std::vector<gr_fir_fsf_info> *info)
{
  gr_fir_sysconfig_singleton()->get_gr_fir_fsf_info (info);
}
