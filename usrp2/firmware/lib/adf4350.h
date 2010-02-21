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

#ifndef ADF4350_H
#define ADF4350_H

#include <db_base.h>
#include <stdbool.h>
#include <stdint.h>

#define DIV_ROUND(num, denom) (((num) + ((denom)/2))/(denom))
#define UINT8_C(num) ((uint8_t) (num))
#define UINT16_C(num) ((uint16_t) (num))

void adf4350_init(struct db_base *dbb);
//void adf4350_update(void);
bool adf4350_get_locked(struct db_base *dbb);
void adf4350_enable(bool enable, struct db_base *dbb);
void adf4350_write(uint8_t addr, uint32_t data, struct db_base *dbb);
bool adf4350_set_freq(u2_fxpt_freq_t freq, struct db_base *dbb);
u2_fxpt_freq_t adf4350_get_freq(struct db_base *dbb);
u2_fxpt_freq_t adf4350_get_max_freq(void);
u2_fxpt_freq_t adf4350_get_min_freq(void);

#endif /* ADF4350_H */
