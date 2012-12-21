/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef GR_PDU_H
#define GR_PDU_H

#include <gr_complex.h>
#include <gruel/pmt.h>

#define pdu_port_id     pmt::mp("pdus")
#define pdu_length_tag  pmt::mp("pdu_length")

enum gr_pdu_vector_type { pdu_byte, pdu_float, pdu_complex };

size_t gr_pdu_itemsize(gr_pdu_vector_type type);
bool gr_pdu_type_matches(gr_pdu_vector_type type, pmt::pmt_t v);
pmt::pmt_t gr_pdu_make_vector(gr_pdu_vector_type type, const uint8_t* buf, size_t items);
gr_pdu_vector_type type_from_pmt(pmt::pmt_t vector);

#endif
