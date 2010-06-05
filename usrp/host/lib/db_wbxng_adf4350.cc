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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "db_wbxng_adf4350.h"
#include <db_base_impl.h>
#include <stdio.h>

#define FREQ_C(freq) uint64_t(freq)
#define DIV_ROUND(num, denom) (((num) + ((denom)/2))/(denom))
#define MIN_INT_DIV uint16_t(23)                                        /* minimum int divider, prescaler 4/5 only */
#define MAX_RF_DIV uint8_t(16)                                          /* max rf divider, divides rf output */
#define MIN_VCO_FREQ FREQ_C(2.2e9)                                      /* minimum vco freq */
#define MAX_VCO_FREQ FREQ_C(4.4e9)                                      /* minimum vco freq */
#define MAX_FREQ DIV_ROUND(MAX_VCO_FREQ, 1)                                           /* upper bound freq (rf div = 1) */
#define MIN_FREQ DIV_ROUND(MIN_VCO_FREQ, MAX_RF_DIV)                    /* calculated lower bound freq */

#define CE_PIN        (1 << 3)
#define PDB_RF_PIN    (1 << 2)
#define MUX_PIN       (1 << 1)
#define LD_PIN        (1 << 0)

adf4350::adf4350()
{
    d_regs = new adf4350_regs();
}

adf4350::~adf4350()
{
    delete d_regs;
}

std::string
adf4350::compute_register(uint8_t addr)
{
    uint32_t data = d_regs->compute_register(addr);

    data |= addr;

    // create std::string from data here
    char s[4];
    s[0] = (char)((data >> 24) & 0xff);
    s[1] = (char)((data >> 16) & 0xff);
    s[2] = (char)((data >>  8) & 0xff);
    s[3] = (char)(data & 0xff);
    return std::string(s, 4);
}

freq_t
adf4350::_get_max_freq(void)
{
    return MAX_FREQ;
}

freq_t
adf4350::_get_min_freq(void)
{
    return MIN_FREQ;
}

bool
adf4350::_set_freq(freq_t freq, freq_t refclock_freq)
{
    /* Set the frequency by setting int, frac, mod, r, div */
    if (freq > MAX_FREQ || freq < MIN_FREQ) return false;
    int min_int_div = 23;
    d_regs->d_prescaler = 0;
    if (freq > FREQ_C(3e9)) {
        min_int_div = 75;
        d_regs->d_prescaler = 1;
    }
    /* Ramp up the RF divider until the VCO is within range. */
    d_regs->d_divider_select = 0;
    while (freq < MIN_VCO_FREQ){
        freq <<= 1; //double the freq
        d_regs->d_divider_select++; //double the divider
    }
    /* Ramp up the R divider until the N divider is at least the minimum. */
    //d_regs->d_10_bit_r_counter = refclock_freq*MIN_INT_DIV/freq;
    d_regs->d_10_bit_r_counter = 2;
    uint64_t n_mod;
    do{
        d_regs->d_10_bit_r_counter++;
        n_mod = freq;
        n_mod *= d_regs->d_10_bit_r_counter;
        n_mod *= d_regs->d_mod;
        n_mod /= refclock_freq;
        /* calculate int and frac */
        d_regs->d_int = n_mod/d_regs->d_mod;
        d_regs->d_frac = (n_mod - (freq_t)d_regs->d_int*d_regs->d_mod) & uint16_t(0xfff);
        /*
        fprintf(stderr,
            "VCO %lu KHz, Int %u, Frac %u, Mod %u, R %u, Div %u\n",
            freq, d_regs->d_int, d_regs->d_frac,
            d_regs->d_mod, d_regs->d_10_bit_r_counter, (1 << d_regs->d_divider_select)
        );
        */
    }while(d_regs->d_int < min_int_div);
    /* calculate the band select so PFD is under 125 KHz */
    d_regs->d_8_bit_band_select_clock_divider_value = \
        refclock_freq/(FREQ_C(30e3)*d_regs->d_10_bit_r_counter) + 1;
    /*
    fprintf(stderr, "Band Selection: Div %u, Freq %lu\n",
        d_regs->d_8_bit_band_select_clock_divider_value,
        refclock_freq/(d_regs->d_8_bit_band_select_clock_divider_value * d_regs->d_10_bit_r_counter) + 1
    );
    */
    return true;
}

freq_t
adf4350::_get_freq(freq_t refclock_freq)
{
    /* Calculate the freq from int, frac, mod, ref, r, div:
     *  freq = (int + frac/mod) * (ref/r)
     * Keep precision by doing multiplies first:
     *  freq = (((((((int)*mod) + frac)*ref)/mod)/r)/div)
     */
    uint64_t freq;
    freq = d_regs->d_int;
    freq *= d_regs->d_mod;
    freq += d_regs->d_frac;
    freq *= refclock_freq;
    freq /= d_regs->d_mod;
    freq /= d_regs->d_10_bit_r_counter;
    freq /= (1 << d_regs->d_divider_select);
    return freq;
}
