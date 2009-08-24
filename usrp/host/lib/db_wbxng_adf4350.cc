/*
 * Copyright 2009 Ettus Research LLC
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp/db_wbxng_adf4350.h>
#include <usrp/db_wbxng_adf4350_regs.h>
#include <db_base_impl.h>
#include <stdio.h>
//#include "io.h"
//#include "spi.h"

#define INPUT_REF_FREQ FREQ_C(32e6)
#define DIV_ROUND(num, denom) (((num) + ((denom)/2))/(denom))
#define FREQ_C(freq) ((uint64_t)DIV_ROUND(freq, (uint64_t)1000))
#define INPUT_REF_FREQ_2X (2*INPUT_REF_FREQ)                            /* input ref freq with doubler turned on */
#define MIN_INT_DIV uint16_t(300)                                        /* minimum int divider, prescaler 4/5 only */
#define MAX_RF_DIV uint8_t(16)                                          /* max rf divider, divides rf output */
#define MIN_VCO_FREQ FREQ_C(2.2e9)                                      /* minimum vco freq */
#define MAX_VCO_FREQ FREQ_C(4.4e9)                                      /* minimum vco freq */
#define MAX_FREQ MAX_VCO_FREQ                                           /* upper bound freq (rf div = 1) */
#define MIN_FREQ DIV_ROUND(MIN_VCO_FREQ, MAX_RF_DIV)                    /* calculated lower bound freq */

#define CE_PIN        (1 << 3)
#define PDB_RF_PIN    (1 << 2)
#define MUX_PIN       (1 << 1)
#define LD_PIN        (1 << 0)

adf4350::adf4350(usrp_basic_sptr _usrp, int _which, int _spi_enable){
	/* Initialize the pin directions. */

    d_usrp = _usrp;
    d_which = _which;
    d_spi_enable = _spi_enable;
    d_spi_format = SPI_FMT_MSB | SPI_FMT_HDR_0;

    d_regs = new adf4350_regs(this);

    /* Outputs */
    d_usrp->_write_oe(d_which, (CE_PIN | PDB_RF_PIN), (CE_PIN | PDB_RF_PIN));
    d_usrp->write_io(d_which, (CE_PIN), (CE_PIN | PDB_RF_PIN));

	/* Initialize the pin levels. */
	_enable(true);
	/* Initialize the registers. */
    /*
    timespec t;
    t.tv_sec = 1;
    t.tv_nsec = 0;
    while (1) {
    */
    d_regs->_load_register(5);
    /*
        nanosleep(&t, NULL);
    }
    */
	d_regs->_load_register(4);
	d_regs->_load_register(3);
	d_regs->_load_register(2);
	d_regs->_load_register(1);
	d_regs->_load_register(0);
}

adf4350::~adf4350(){
    delete d_regs;
}

freq_t 
adf4350::_get_max_freq(void){
	return MAX_FREQ;
}

freq_t 
adf4350::_get_min_freq(void){
	return MIN_FREQ;
}

bool 
adf4350::_get_locked(void){
    return d_usrp->read_io(d_which) & LD_PIN;
}

void 
adf4350::_enable(bool enable){
	if (enable){ /* chip enable */
        d_usrp->write_io(d_which, (CE_PIN | PDB_RF_PIN), (CE_PIN | PDB_RF_PIN));
	}else{
        d_usrp->write_io(d_which, 0, (CE_PIN | PDB_RF_PIN));
	}
}

void 
adf4350::_write(uint8_t addr, uint32_t data){
	data |= addr;

    // create str from data here
    char s[4];
    s[0] = (char)((data >> 24) & 0xff);
    s[1] = (char)((data >> 16) & 0xff);
    s[2] = (char)((data >>  8) & 0xff);
    s[3] = (char)(data & 0xff);
    std::string str(s, 4);

    d_usrp->_write_spi(0, d_spi_enable, d_spi_format, str);
    fprintf(stderr, "Wrote to WBXNG SPI address %d with data %8x\n", addr, data);
	/* pulse latch */
    //d_usrp->write_io(d_which, 1, LE_PIN);
    //d_usrp->write_io(d_which, 0, LE_PIN);
}

bool 
adf4350::_set_freq(freq_t freq){
	/* Set the frequency by setting int, frac, mod, r, div */
	if (freq > MAX_FREQ || freq < MIN_FREQ) return false;
	/* Ramp up the RF divider until the VCO is within range. */
	d_regs->d_divider_select = 0;
	while (freq < MIN_VCO_FREQ){
		freq <<= 1; //double the freq
		d_regs->d_divider_select++; //double the divider
	}
	/* Ramp up the R divider until the N divider is at least the minimum. */
	d_regs->d_10_bit_r_counter = INPUT_REF_FREQ*MIN_INT_DIV/freq;
	uint64_t n_mod;
	do{
		d_regs->d_10_bit_r_counter++;
		n_mod = freq;
		n_mod *= d_regs->d_10_bit_r_counter;
		n_mod *= d_regs->d_mod;
		n_mod /= INPUT_REF_FREQ;
		/* calculate int and frac */
		d_regs->d_int = n_mod/d_regs->d_mod;
		d_regs->d_frac = (n_mod - (freq_t)d_regs->d_int*d_regs->d_mod) & uint16_t(0xfff);
		fprintf(stderr,
			"VCO %lu KHz, Int %u, Frac %u, Mod %u, R %u, Div %u\n",
			freq, d_regs->d_int, d_regs->d_frac,
			d_regs->d_mod, d_regs->d_10_bit_r_counter, (1 << d_regs->d_divider_select)
		);
	}while(d_regs->d_int < MIN_INT_DIV);
	/* calculate the band select so PFD is under 125 KHz */
	d_regs->d_8_bit_band_select_clock_divider_value = \
		INPUT_REF_FREQ/(FREQ_C(125e3)*d_regs->d_10_bit_r_counter) + 1;
	/* load involved registers */
	d_regs->_load_register(2);
	d_regs->_load_register(4);
	d_regs->_load_register(0); /* register 0 must be last */
	return true;
}

freq_t 
adf4350::_get_freq(void){
	/* Calculate the freq from int, frac, mod, ref, r, div:
	 *  freq = (int + frac/mod) * (ref/r)
	 * Keep precision by doing multiplies first:
	 *  freq = (((((((int)*mod) + frac)*ref)/mod)/r)/div)
	 */
	uint64_t temp;
	temp = d_regs->d_int;
	temp *= d_regs->d_mod;
	temp += d_regs->d_frac;
	temp *= INPUT_REF_FREQ;
	temp /= d_regs->d_mod;
	temp /= d_regs->d_10_bit_r_counter;
	temp /= (1 << d_regs->d_divider_select);
	return temp;
}
