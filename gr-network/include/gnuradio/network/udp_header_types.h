/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef NETWORK_UDPHEADERTYPES_H_
#define NETWORK_UDPHEADERTYPES_H_

constexpr int HEADERTYPE_NONE = 0;
constexpr int HEADERTYPE_SEQNUM = 1;
constexpr int HEADERTYPE_SEQPLUSSIZE = 2;
// 3 and 4 were defined but have been removed.  ATA kept as 5 for backward compatibility.
constexpr int HEADERTYPE_OLDATA = 5;

#endif /* NETWORK_UDPHEADERTYPES_H_ */
