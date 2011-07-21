/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_FIR_SYSCONFIG_POWERPC_H
#define INCLUDED_GR_FIR_SYSCONFIG_POWERPC_H

#include <gr_core_api.h>
#include <gr_fir_sysconfig_generic.h>

class GR_CORE_API gr_fir_sysconfig_powerpc : public gr_fir_sysconfig_generic {
public:
  virtual gr_fir_ccf *create_gr_fir_ccf (const std::vector<float> &taps);
  virtual gr_fir_fcc *create_gr_fir_fcc (const std::vector<gr_complex> &taps);
  virtual gr_fir_fff *create_gr_fir_fff (const std::vector<float> &taps);
  virtual gr_fir_fsf *create_gr_fir_fsf (const std::vector<float> &taps);
  virtual gr_fir_scc *create_gr_fir_scc (const std::vector<gr_complex> &taps);
  virtual gr_fir_ccc *create_gr_fir_ccc (const std::vector<gr_complex> &taps);
//virtual gr_fir_sss *create_gr_fir_sss (const std::vector<short> &taps);

  virtual void get_gr_fir_ccf_info (std::vector<gr_fir_ccf_info> *info);
  virtual void get_gr_fir_fcc_info (std::vector<gr_fir_fcc_info> *info);
  virtual void get_gr_fir_fff_info (std::vector<gr_fir_fff_info> *info);
  virtual void get_gr_fir_fsf_info (std::vector<gr_fir_fsf_info> *info);
  virtual void get_gr_fir_scc_info (std::vector<gr_fir_scc_info> *info);
  virtual void get_gr_fir_ccc_info (std::vector<gr_fir_ccc_info> *info);
//virtual void get_gr_fir_sss_info (std::vector<gr_fir_sss_info> *info);
};

#endif
