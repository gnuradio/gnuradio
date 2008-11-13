/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

// @WARNING@

GR_SWIG_BLOCK_MAGIC(trellis,@BASE_NAME@);

@SPTR_NAME@ trellis_make_@BASE_NAME@ (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<@I_TYPE@> &TABLE,
    trellis_metric_type_t TYPE);


class @NAME@ : public gr_block
{
private:
  @NAME@ (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<@I_TYPE@> &TABLE,
    trellis_metric_type_t TYPE);

public:
    fsm FSM () const { return d_FSM; }
    int K () const { return d_K; }
    int S0 () const { return d_S0; }
    int SK () const { return d_SK; }
    int D () const { return d_D; }
    std::vector<@I_TYPE@> TABLE () const { return d_TABLE; }
    trellis_metric_type_t TYPE () const { return d_TYPE; }
    //std::vector<short> trace () const { return d_trace; }
    void set_TABLE (const std::vector<@I_TYPE@> &table);
};
