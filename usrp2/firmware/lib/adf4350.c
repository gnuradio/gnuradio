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

#include "adf4350.h"
#include "adf4350_regs.h"
#include "db_wbxng.h"
#include <spi.h>
#include <hal_io.h>
#include <stdio.h>
#include <stdint.h>

#define INPUT_REF_FREQ U2_DOUBLE_TO_FXPT_FREQ(50e6)
#define INPUT_REF_FREQ_2X (2*INPUT_REF_FREQ)                            /* input ref freq with doubler turned on */
#define MAX_RF_DIV UINT8_C(16)                                          /* max rf divider, divides rf output */
#define MIN_VCO_FREQ U2_DOUBLE_TO_FXPT_FREQ(2.2e9)                      /* minimum vco freq */
#define MAX_VCO_FREQ U2_DOUBLE_TO_FXPT_FREQ(4.4e9)                      /* minimum vco freq */
#define MAX_FREQ MAX_VCO_FREQ                                           /* upper bound freq (rf div = 1) */
#define MIN_FREQ U2_DOUBLE_TO_FXPT_FREQ(68.75e6)                        /* lower bound freq (rf div = 16) */

u2_fxpt_freq_t adf4350_get_max_freq(void){
	return MAX_FREQ;
}

u2_fxpt_freq_t adf4350_get_min_freq(void){
	return MIN_FREQ;
}

void adf4350_init(struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	/* Initialize the pin levels. */
	hal_gpio_write( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK, PLL_CE, PLL_CE );
	adf4350_enable(true, dbb);
	/* Initialize the registers. */
	adf4350_load_register(5, dbb);
	adf4350_load_register(4, dbb);
	adf4350_load_register(3, dbb);
	adf4350_load_register(2, dbb);
	adf4350_load_register(1, dbb);
	adf4350_load_register(0, dbb);
}

/*
void adf4350_update(void){
	// mirror the lock detect pin to the led debug
	if (adf4350_get_locked()){
		io_set_pin(led_pin);
	}else{
		io_clear_pin(led_pin);
	}
}
*/

bool adf4350_get_locked(struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	int pins;
	pins = hal_gpio_read( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK );
  	if(pins & PLL_LOCK_DETECT)
		return true;
	return false;
}

void adf4350_enable(bool enable, struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	if (enable){ /* chip enable */
		hal_gpio_write( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK, PLL_PDBRF, PLL_PDBRF );
	}else{
		hal_gpio_write( db->base.is_tx ? GPIO_TX_BANK : GPIO_RX_BANK, 0, PLL_PDBRF );
	}
}

void adf4350_write(uint8_t addr, uint32_t data, struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	//printf("SPI write ADDR 0x%x, WORD 0x%x\n", (int) (addr), (int) (data));
	data |= addr;
	spi_transact(SPI_TXONLY,db->common.spi_mask,data,32,SPIF_PUSH_FALL);
	//spi_read_write(clk_pin, data_pin, ld_pin, &data, 32);
	/* pulse latch */
	//io_set_pin(le_pin);
	//io_clear_pin(le_pin);
}

bool adf4350_set_freq(u2_fxpt_freq_t freq, struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	/* Set the frequency by setting int, frac, mod, r, div */
	if (freq > MAX_FREQ || freq < MIN_FREQ) return false;

	/* Set the prescaler and the min N based on the freq. */
	uint16_t min_int_div;
	if (freq > U2_DOUBLE_TO_FXPT_FREQ(3e9) ){
		db->common.adf4350_regs_prescaler = (uint8_t) 1;
		min_int_div = UINT16_C(75);
	}else{
		db->common.adf4350_regs_prescaler = (uint8_t) 0;
		min_int_div = UINT16_C(23);
	}

	/* Ramp up the RF divider until the VCO is within range. */
	db->common.adf4350_regs_divider_select = (uint8_t) 0;
	while (freq < MIN_VCO_FREQ){
		freq <<= 1; //double the freq
		db->common.adf4350_regs_divider_select++; //double the divider
	}

	/* Ramp up the R divider until the N divider is at least the minimum. */
	db->common.adf4350_regs_10_bit_r_counter = (uint16_t) (DIV_ROUND((INPUT_REF_FREQ*min_int_div), freq));
	//printf("Initial R setting: %u, MIN_INT: %u\n", db->common.adf4350_regs_10_bit_r_counter, min_int_div);
	if (db->common.adf4350_regs_10_bit_r_counter * U2_DOUBLE_TO_FXPT_FREQ(32e6) < INPUT_REF_FREQ){
		db->common.adf4350_regs_10_bit_r_counter = (uint16_t) (DIV_ROUND(INPUT_REF_FREQ, U2_DOUBLE_TO_FXPT_FREQ(32e6)));
		//printf("Updating R setting: %u, MIN_INT: %u\n", db->common.adf4350_regs_10_bit_r_counter, min_int_div);
	}

	db->common.adf4350_regs_10_bit_r_counter--;
	//db->common.adf4350_regs_10_bit_r_counter=1;

	do{
		db->common.adf4350_regs_10_bit_r_counter++;
		/* throw out some fractional bits in freq to avoid overflow */
		u2_fxpt_freq_t some_frac_freq = (U2_DOUBLE_TO_FXPT_FREQ(1.0)/db->common.adf4350_regs_mod);
		uint64_t n_mod = DIV_ROUND(freq, some_frac_freq);
		n_mod *= db->common.adf4350_regs_10_bit_r_counter;
		n_mod *= db->common.adf4350_regs_mod;
		n_mod = DIV_ROUND(n_mod, DIV_ROUND(INPUT_REF_FREQ, some_frac_freq));
		/* calculate int and frac: regs_mod is a power of 2, this will optimize to a bitwise operation */
		db->common.adf4350_regs_int = (uint16_t) (n_mod/db->common.adf4350_regs_mod);
		db->common.adf4350_regs_frac = (uint16_t) (n_mod%db->common.adf4350_regs_mod);
		//printf("Int %u < Min %u\n", db->common.adf4350_regs_int, min_int_div);
	}while(db->common.adf4350_regs_int < min_int_div);

	/* calculate the band select so PFD is under 125 KHz */
	db->common.adf4350_regs_8_bit_band_select_clock_divider_value = \
		(uint8_t) (INPUT_REF_FREQ/(U2_DOUBLE_TO_FXPT_FREQ(30e3)*db->common.adf4350_regs_10_bit_r_counter)) + 1;

	/*
	printf(
		"VCO %u KHz, Int %u, Frac %u, Mod %u, R %u, Div %u, BandSelect %u\n",
		(uint32_t) ((freq >> U2_FPF_RP)/1000),
		(uint32_t) db->common.adf4350_regs_int,
		(uint32_t) db->common.adf4350_regs_frac,
		(uint32_t) db->common.adf4350_regs_mod,
		(uint32_t) db->common.adf4350_regs_10_bit_r_counter,
		(uint32_t) (1 << db->common.adf4350_regs_divider_select),
		(uint32_t) db->common.adf4350_regs_8_bit_band_select_clock_divider_value
	);
	*/

	/* load involved registers */
	adf4350_load_register(5, dbb);
	adf4350_load_register(3, dbb);
	adf4350_load_register(1, dbb);
	adf4350_load_register(2, dbb);
	adf4350_load_register(4, dbb);
	adf4350_load_register(0, dbb); /* register 0 must be last */
	return adf4350_get_locked(dbb);
}

u2_fxpt_freq_t adf4350_get_freq(struct db_base *dbb){
	struct db_wbxng_dummy *db = (struct db_wbxng_dummy *) dbb;

	/* Calculate the freq from int, frac, mod, ref, r, div:
	 *  freq = (int + frac/mod) * (ref/r)
	 * Keep precision by doing multiplies first:
	 *  freq = (((((((int)*mod) + frac)*ref)/mod)/r)/div)
	 */
	uint64_t temp;
	temp = (uint64_t) db->common.adf4350_regs_int;
	temp *= (uint64_t) db->common.adf4350_regs_mod;
	temp += (uint64_t) db->common.adf4350_regs_frac;
	temp *= (uint64_t) (INPUT_REF_FREQ >> U2_FPF_RP);
	temp /= (uint64_t) db->common.adf4350_regs_mod;
	temp /= (uint64_t) db->common.adf4350_regs_10_bit_r_counter;
	temp /= (uint64_t) (1 << db->common.adf4350_regs_divider_select);

	/* Shift 1Hz Radix Point for u2_fxpt_freq_t */
	temp = temp << U2_FPF_RP;

	/*
	printf(
		"Got Freq %u KHz, Int %u, Frac %u, Mod %u, R %u, Div %u\n",
		(uint32_t) ((temp >> U2_FPF_RP)/1000),
		(uint32_t) db->common.adf4350_regs_int,
		(uint32_t) db->common.adf4350_regs_frac,
		(uint32_t) db->common.adf4350_regs_mod,
		(uint32_t) db->common.adf4350_regs_10_bit_r_counter,
		(uint32_t) (1 << db->common.adf4350_regs_divider_select)
	);
	*/

	return (u2_fxpt_freq_t) (temp);
}
