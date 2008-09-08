/* -*- c -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
 */

#ifndef INCLUDED_ETH_MAC_H
#define INCLUDED_ETH_MAC_H

#include "usrp2_mac_addr.h"

void eth_mac_init(const u2_mac_addr_t *src);

void eth_mac_set_addr(const u2_mac_addr_t *src);
int  eth_mac_read_rmon(int addr);
int  eth_mac_miim_read(int addr);
void eth_mac_miim_write(int addr, int value);
int  eth_mac_miim_read_status(void);

#endif /* INCLUDED_ETH_MAC_H */
