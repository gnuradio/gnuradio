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
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <db_boards.h>
#include <usrp/usrp_dbid.h>
#include <usrp/db_basic.h>
#include <usrp/db_tv_rx.h>
#include <usrp/db_tv_rx_mimo.h>
#include <usrp/db_dbs_rx.h>
#include <usrp/db_flexrf.h>
#include <usrp/db_flexrf_mimo.h>
#include <usrp/db_wbxng.h>
#include <usrp/db_xcvr2450.h>
#include <usrp/db_dtt754.h>
#include <usrp/db_dtt768.h>
#include <usrp/db_bitshark_rx.h>
#include <cstdio>

std::vector<db_base_sptr>
instantiate_dbs(int dbid, usrp_basic_sptr usrp, int which_side)
{
  std::vector<db_base_sptr> db;

  switch(dbid) {

  case(USRP_DBID_BASIC_TX):
    db.push_back(db_base_sptr(new db_basic_tx(usrp, which_side)));
    break;

  case(USRP_DBID_BASIC_RX):
    db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 0)));
    db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 1)));
    db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 2)));
    break;

  case(USRP_DBID_LF_TX):
    db.push_back(db_base_sptr(new db_lf_tx(usrp, which_side)));
    break;

  case(USRP_DBID_LF_RX):
    db.push_back(db_base_sptr(new db_lf_rx(usrp, which_side, 0)));
    db.push_back(db_base_sptr(new db_lf_rx(usrp, which_side, 1)));
    db.push_back(db_base_sptr(new db_lf_rx(usrp, which_side, 2)));
    break;

  case(USRP_DBID_DBS_RX):
    db.push_back(db_base_sptr(new db_dbs_rx(usrp, which_side)));
    break;

  case(USRP_DBID_TV_RX):
    db.push_back(db_base_sptr(new db_tv_rx(usrp, which_side, 43.75e6, 5.75e6)));
    break;
  case(USRP_DBID_TV_RX_REV_2):
    db.push_back(db_base_sptr(new db_tv_rx(usrp, which_side, 44e6, 20e6)));
    break;
  case(USRP_DBID_TV_RX_REV_3):
    db.push_back(db_base_sptr(new db_tv_rx(usrp, which_side, 44e6, 20e6)));
    break;
  case(USRP_DBID_TV_RX_MIMO):
    db.push_back(db_base_sptr(new db_tv_rx_mimo(usrp, which_side, 43.75e6, 5.75e6)));
    break;
  case(USRP_DBID_TV_RX_REV_2_MIMO):
    db.push_back(db_base_sptr(new db_tv_rx_mimo(usrp, which_side, 44e6, 20e6)));
    break;
  case(USRP_DBID_TV_RX_REV_3_MIMO):
    db.push_back(db_base_sptr(new db_tv_rx_mimo(usrp, which_side, 44e6, 20e6)));
    break;

  case(USRP_DBID_FLEX_2400_TX):
    db.push_back(db_base_sptr(new db_flexrf_2400_tx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_2400_RX):
    db.push_back(db_base_sptr(new db_flexrf_2400_rx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_TX):
    db.push_back(db_base_sptr(new db_flexrf_1200_tx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_RX):
    db.push_back(db_base_sptr(new db_flexrf_1200_rx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_TX):
    db.push_back(db_base_sptr(new db_flexrf_1800_tx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_RX):
    db.push_back(db_base_sptr(new db_flexrf_1800_rx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_TX):
    db.push_back(db_base_sptr(new db_flexrf_900_tx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_RX):
    db.push_back(db_base_sptr(new db_flexrf_900_rx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_TX):
    db.push_back(db_base_sptr(new db_flexrf_400_tx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_RX):
    db.push_back(db_base_sptr(new db_flexrf_400_rx(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_2400_TX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_2400_tx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_2400_RX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_2400_rx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_TX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_1800_tx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_RX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_1800_rx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_TX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_1200_tx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_RX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_1200_rx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_TX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_900_tx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_RX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_900_rx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_TX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_400_tx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_RX_MIMO_A):
    db.push_back(db_base_sptr(new db_flexrf_400_rx_mimo_a(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_2400_TX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_2400_tx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_2400_RX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_2400_rx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_TX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_1800_tx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1800_RX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_1800_rx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_TX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_1200_tx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_1200_RX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_1200_rx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_TX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_900_tx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_900_RX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_900_rx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_TX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_400_tx_mimo_b(usrp, which_side)));
    break;
  case(USRP_DBID_FLEX_400_RX_MIMO_B):
    db.push_back(db_base_sptr(new db_flexrf_400_rx_mimo_b(usrp, which_side)));
    break;

  case(USRP_DBID_XCVR2450_TX):
    db.push_back(db_base_sptr(new db_xcvr2450_tx(usrp, which_side)));
    break;
  case(USRP_DBID_XCVR2450_RX):
    db.push_back(db_base_sptr(new db_xcvr2450_rx(usrp, which_side)));
    break;

#if 0	// FIXME wbx doesn't compile
  case(USRP_DBID_WBX_LO_TX):
    db.push_back(db_base_sptr(new db_wbx_lo_tx(usrp, which_side)));
    break;
  case(USRP_DBID_WBX_LO_RX):
    db.push_back(db_base_sptr(new db_wbx_lo_rx(usrp, which_side)));
    break;
#endif

  case(USRP_DBID_WBX_NG_TX):
    db.push_back(db_base_sptr(new db_wbxng_tx(usrp, which_side)));
    break;
  case(USRP_DBID_WBX_NG_RX):
    db.push_back(db_base_sptr(new db_wbxng_rx(usrp, which_side)));
    break;

  case(USRP_DBID_DTT754):
    db.push_back(db_base_sptr(new db_dtt754(usrp, which_side)));
    break;
  case(USRP_DBID_DTT768):
    db.push_back(db_base_sptr(new db_dtt768(usrp, which_side)));
    break;

  case(USRP_DBID_BITSHARK_RX):
    db.push_back(db_base_sptr(new db_bitshark_rx(usrp, which_side)));
    break;

  case(-1):
    if (boost::dynamic_pointer_cast<usrp_basic_tx>(usrp)){
      db.push_back(db_base_sptr(new db_basic_tx(usrp, which_side)));
    }
    else {
      db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 0)));
      db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 1)));
    }
    break;

  case(-2):
  default:
    if (boost::dynamic_pointer_cast<usrp_basic_tx>(usrp)){
      fprintf(stderr, "\n\aWarning: Treating daughterboard with invalid EEPROM contents as if it were a \"Basic Tx.\"\n");
      fprintf(stderr, "Warning: This is almost certainly wrong...  Use appropriate burn-*-eeprom utility.\n\n");
      db.push_back(db_base_sptr(new db_basic_tx(usrp, which_side)));
    }
    else {
      fprintf(stderr, "\n\aWarning: Treating daughterboard with invalid EEPROM contents as if it were a \"Basic Rx.\"\n");
      fprintf(stderr, "Warning: This is almost certainly wrong...  Use appropriate burn-*-eeprom utility.\n\n");
      db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 0)));
      db.push_back(db_base_sptr(new db_basic_rx(usrp, which_side, 1)));
    }
    break;
  }

  return db;
}
