/* -*- c++ -*- */
//
// Copyright 2008,2009 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either asversion 3, or (at your option)
// any later version.
// 
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.

#ifndef DB_DBS_RX_H
#define DB_DBS_RX_H

#include <usrp/db_base.h>
#include <vector>

#if 0
struct bw_t {
  int m;
  int fdac;
  float div;
};
#endif

class db_dbs_rx : public db_base
{
private:
  int d_osc, d_cp, d_n, d_div2, d_r, d_r_int;
  int d_fdac, d_m, d_dl, d_ade, d_adl, d_gc1, d_gc2, d_diag;
  int d_i2c_addr;
  
  // Internal gain functions
  void _write_reg(int regno, int v);
  void _write_regs(int starting_regno, const std::vector<int> &vals);
  std::vector<int> _read_status();
  void _send_reg(int regno);
  void _set_m(int m);
  void _set_fdac(int fdac);
  void _set_dl(int dl);
  void _set_gc2(int gc2);
  void _set_gc1(int gc1);
  void _set_pga(int pga_gain);

  // Internal frequency function
  void _set_osc(int osc);
  void _set_cp(int cp);
  void _set_n(int n);
  void _set_div2(int div2);
  void _set_r(int r);
  void _set_ade(int ade);

  int _refclk_divisor();

protected:
  void shutdown();

public:
  db_dbs_rx(usrp_basic_sptr usrp, int which);
  ~db_dbs_rx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  double freq_min();
  double freq_max();
  struct freq_result_t set_freq(double freq);
  bool  set_gain(float gain);
  bool  is_quadrature();
  bool  set_bw(float bw);
};

#endif
