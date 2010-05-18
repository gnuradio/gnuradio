/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DB_BITSHARK_RX_H
#define DB_BITSHARK_RX_H

#include <db_base.h>

struct db_bitshark_rx_extra 
{
    uint16_t bw_min;
    uint16_t bw_max;
    bool     (*set_bw)(struct db_base *, uint16_t bw);
	
};

struct db_bitshark_rx_dummy 
{
  struct db_base	base;
  struct db_bitshark_rx_extra	extra;
};


struct db_bitshark_rx 
{
  struct db_base	base;
  struct db_bitshark_rx_extra extra;
};


#endif /* DB_BITSHARK_RX_H */
