//
// Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ADF4350_H
#define INCLUDED_ADF4350_H

#include "db_wbxng_adf4350_regs.h"
#include <usrp/db_base.h>
#include <stdint.h>

typedef uint64_t freq_t;
class adf4350_regs;

class adf4350
{
public:
    adf4350();
    ~adf4350();
    void _update();
    std::string compute_register(uint8_t addr);
    bool _set_freq(freq_t freq, freq_t refclock_freq);
    freq_t _get_freq(freq_t refclock_freq);
    freq_t _get_max_freq();
    freq_t _get_min_freq();

protected:
    adf4350_regs *d_regs;
};

#endif /* INCLUDED_ADF4350_H */
