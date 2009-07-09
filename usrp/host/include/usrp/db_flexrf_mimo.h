/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#include <usrp/db_flexrf.h>

class db_flexrf_2400_tx_mimo_a : public db_flexrf_2400_tx
{
 public:
  db_flexrf_2400_tx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_2400_rx_mimo_a : public db_flexrf_2400_rx
{
 public:
  db_flexrf_2400_rx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};
      
class db_flexrf_2400_tx_mimo_b : public db_flexrf_2400_tx
{
 public:
  db_flexrf_2400_tx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_2400_rx_mimo_b : public db_flexrf_2400_rx
{
 public:
  db_flexrf_2400_rx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};


class db_flexrf_1800_tx_mimo_a : public db_flexrf_1800_tx
{
 public:
  db_flexrf_1800_tx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1800_rx_mimo_a : public db_flexrf_1800_rx
{
 public:
  db_flexrf_1800_rx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1800_tx_mimo_b : public db_flexrf_1800_tx
{
 public:
  db_flexrf_1800_tx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1800_rx_mimo_b : public db_flexrf_1800_rx
{
 public:
  db_flexrf_1800_rx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1200_tx_mimo_a : public db_flexrf_1200_tx
{
 public:
  db_flexrf_1200_tx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1200_rx_mimo_a : public db_flexrf_1200_rx
{
 public:
  db_flexrf_1200_rx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1200_tx_mimo_b : public db_flexrf_1200_tx
{
 public:
  db_flexrf_1200_tx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_1200_rx_mimo_b : public db_flexrf_1200_rx
{
 public:
  db_flexrf_1200_rx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_900_tx_mimo_a : public db_flexrf_900_tx
{
 public:
  db_flexrf_900_tx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_900_rx_mimo_a : public db_flexrf_900_rx
{
 public:
  db_flexrf_900_rx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_900_tx_mimo_b : public db_flexrf_900_tx
{
 public:
  db_flexrf_900_tx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_900_rx_mimo_b : public db_flexrf_900_rx
{
 public:
  db_flexrf_900_rx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_400_tx_mimo_a : public db_flexrf_400_tx
{
 public:
  db_flexrf_400_tx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_400_rx_mimo_a : public db_flexrf_400_rx
{
 public:
  db_flexrf_400_rx_mimo_a(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_400_tx_mimo_b : public db_flexrf_400_tx
{
 public:
  db_flexrf_400_tx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};

class db_flexrf_400_rx_mimo_b : public db_flexrf_400_rx
{
 public:
  db_flexrf_400_rx_mimo_b(usrp_basic_sptr usrp, int which);
  int _refclk_divisor();
};
