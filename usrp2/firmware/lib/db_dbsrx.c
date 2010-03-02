/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#include <i2c.h>
#include <db_base.h>
#include <lsdac.h>
#include <memory_map.h>
#include <clocks.h>
#include <stdio.h>
#include <hal_io.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define abs(X) ((X) < (0) ? ((-1)*(X)) : (X))

#define I2C_ADDR 0x67
#define REFCLK_DIVISOR 25   // Gives a 4 MHz clock
#define REFCLK_FREQ U2_DOUBLE_TO_FXPT_FREQ(MASTER_CLK_RATE/REFCLK_DIVISOR)
#define REFCLK_FREQ_INT u2_fxpt_freq_round_to_int(REFCLK_FREQ)

#define VMAXGAIN .75
#define VMINGAIN 2.6
#define RFGAINMAX 60
#define BBGAINMAX 24
#define DACFULLSCALE 3.3

bool db_dbsrx_init(struct db_base *db);
bool db_dbsrx_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool db_dbsrx_set_gain(struct db_base *db, u2_fxpt_gain_t gain);

struct db_dbsrx_common {
  int d_n;
  int d_div2;
  int d_osc;
  int d_cp;
  int d_r_reg;
  int d_fdac;
  int d_m;
  int d_dl;
  int d_ade;
  int d_adl;
  int d_gc2;
  int d_diag;
};

struct db_dbsrx_dummy {
  struct db_base base;
  struct db_dbsrx_common common;
};

struct db_dbsrx {
  struct db_base base;
  struct db_dbsrx_common common;
};

struct db_dbsrx db_dbsrx = {
  .base.dbid = 0x000d,
  .base.is_tx = false,
  .base.output_enables = 0x0000,
  .base.used_pins = 0x0000,
  .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(500e6),
  .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(2.6e9),
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(RFGAINMAX+BBGAINMAX),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
  .base.init = db_dbsrx_init,
  .base.set_freq = db_dbsrx_set_freq,
  .base.set_gain = db_dbsrx_set_gain,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x0000,
  .base.atr_txval = 0,
  .base.atr_rxval = 0,
  //.base.atr_tx_delay =
  //.base.atr_rx_delay =
  .common.d_n = 950,
  .common.d_div2 = 0,
  .common.d_osc = 5,
  .common.d_cp = 3,
  .common.d_r_reg = 1,
  .common.d_fdac = 127,
  .common.d_m = 2,
  .common.d_dl = 1,
  .common.d_ade = 0,
  .common.d_adl = 0,
  .common.d_gc2 = 31,
  .common.d_diag = 0,
  .base.set_antenna = 0,
};

bool
db_dbsrx_init(struct db_base *dbb){
  struct db_dbsrx_dummy *db = (struct db_dbsrx_dummy *) dbb;
  db->base.set_gain(dbb, (db->base.gain_max + db->base.gain_min)/2);
  clocks_enable_rx_dboard(true, REFCLK_DIVISOR);  // Gives 4 MHz clock

  return true;
}

/**************************************************
 * Registers
 **************************************************/
static int
_read_adc (void){
  unsigned char readback[2];
  i2c_read(I2C_ADDR, readback, 2*sizeof(unsigned char));
  int adc_val = (readback[0] >> 2)&7;
  //printf("READBACK[0] %d, [1] %d\n",readback[0],readback[1]);
  //printf("ADC: %d\n",adc_val);
  return adc_val;
}

static void
_write_reg (int regno, int v){
  //regno is in [0,5], v is value to write to register"""
  unsigned char args[2];
  args[0] = (unsigned char)regno;
  args[1] = (unsigned char)v;
  i2c_write(I2C_ADDR, args, 2*sizeof(unsigned char));
  //printf("Reg %d, Val %x\n",regno,v);
}

static void _send_reg_0(struct db_dbsrx_dummy *db){
  _write_reg(0,(db->common.d_div2<<7) + (db->common.d_n>>8));
}

static void _send_reg_1(struct db_dbsrx_dummy *db){
  _write_reg(1,db->common.d_n & 255);
}

static void _send_reg_2(struct db_dbsrx_dummy *db){
  _write_reg(2,db->common.d_osc + (db->common.d_cp<<3) + (db->common.d_r_reg<<5));
}

static void _send_reg_3(struct db_dbsrx_dummy *db){
  _write_reg(3,db->common.d_fdac);
}

static void _send_reg_4(struct db_dbsrx_dummy *db){
  _write_reg(4,db->common.d_m + (db->common.d_dl<<5) + (db->common.d_ade<<6) + (db->common.d_adl<<7));
}

static void _send_reg_5(struct db_dbsrx_dummy *db){
  _write_reg(5,db->common.d_gc2 + (db->common.d_diag<<5));
}

/**************************************************
 * Helpers for setting the freq
 **************************************************/
static void
_set_div2(struct db_dbsrx_dummy *db, int div2){
  db->common.d_div2 = div2;
  _send_reg_0(db);
}

// FIXME  How do we handle ADE and ADL properly?
static void
_set_ade(struct db_dbsrx_dummy *db, int ade){
  db->common.d_ade = ade;
  _send_reg_4(db);
}

static void
_set_r(struct db_dbsrx_dummy *db, int r){
  db->common.d_r_reg = r;
  _send_reg_2(db);
}

static void
_set_n(struct db_dbsrx_dummy *db, int n){
  db->common.d_n = n;
  _send_reg_0(db);
  _send_reg_1(db);
}

static void
_set_osc(struct db_dbsrx_dummy *db, int osc){
  db->common.d_osc = osc;
  _send_reg_2(db);
}

static void
_set_cp(struct db_dbsrx_dummy *db, int cp){
  db->common.d_cp = cp;
  _send_reg_2(db);
}

/**************************************************
 * Set the freq
 **************************************************/


bool
db_dbsrx_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc){
  struct db_dbsrx_dummy *db = (struct db_dbsrx_dummy *) dbb;

  if(!(freq>=db->base.freq_min && freq<=db->base.freq_max)) {
    return false;
  }

  u2_fxpt_freq_t vcofreq;
  if(freq < U2_DOUBLE_TO_FXPT_FREQ(1150e6)) {
    _set_div2(db, 0);
    vcofreq = 4 * freq;
  }
  else {
    _set_div2(db, 1);
    vcofreq = 2 * freq;
  }
  
  _set_ade(db, 1);
  int rmin = max(2, u2_fxpt_freq_round_to_int(REFCLK_FREQ/2e6)); //TODO? remove max()
  //int rmax = min(128, u2_fxpt_freq_round_to_int(REFCLK_FREQ/500e3)); //TODO? remove min()
  int n = 0;
  u2_fxpt_freq_t best_delta = U2_DOUBLE_TO_FXPT_FREQ(10e6);
  u2_fxpt_freq_t delta;

  int r_reg = 0;
  while ((r_reg<7) && ((2<<r_reg) < rmin)) {
    r_reg++;
  }
  //printf ("r_reg = %d, r = %d\n",r_reg,2<<r_reg);
  int best_r = r_reg;
  int best_n = 0;

  while(r_reg <= 7) {
    n = u2_fxpt_freq_round_to_int(freq/REFCLK_FREQ_INT*(2<<r_reg));
    //printf("LOOP: r_reg %d, best_r %d, best_n %d, best_delta %d\n",
    //r_reg,best_r,best_n,u2_fxpt_freq_round_to_int(best_delta));

    //printf("N: %d\n",n);
    if(n<256) {
      r_reg++;
      continue;
    }
    delta = abs(n*REFCLK_FREQ/(2<<r_reg) - freq);
    if(delta < best_delta) {
      best_r = r_reg;
      best_n = n;
      best_delta = delta;
    }
    if(best_delta < U2_DOUBLE_TO_FXPT_FREQ(75e3)) {
      break;
    }
    r_reg++;
  }

  //printf("BEST R: %d  Best Delta %d  Best N %d\n",
  // best_r,u2_fxpt_freq_round_to_int(best_delta),best_n);
  _set_r(db, best_r);
  _set_n(db, best_n);
 
  int vco;
  if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(2433e6))
    vco = 0;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(2711e6))
    vco=1;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(3025e6))
    vco=2;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(3341e6))
    vco=3;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(3727e6))
    vco=4;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(4143e6))
    vco=5;
  else if(vcofreq < U2_DOUBLE_TO_FXPT_FREQ(4493e6))
    vco=6;
  else
    vco=7;
  //printf("Initial VCO choice %d\n",vco);  
  _set_osc(db, vco);
  

  int adc_val = 0;
  while(adc_val == 0 || adc_val == 7) {
    adc_val = _read_adc();
    //printf("adc %d\n",adc_val);

    if(adc_val == 0) {
      if(vco <= 0) {
        return false;
      }
      else {
        vco = vco - 1;
      }
    }
    else if(adc_val == 7) {
      if(vco >= 7) {
        return false;
      }
      else {
        vco = vco + 1;
      }
    }
    _set_osc(db, vco);
  }
  
  if(adc_val == 1 || adc_val == 2) {
    _set_cp(db, 1);
  }
  else if(adc_val == 3 || adc_val == 4) {
    _set_cp(db, 2);
  }
  else {
    _set_cp(db, 3);
  }
  //printf("Final VCO choice %d\n",vco);  

  *dc = db->common.d_n * REFCLK_FREQ / (2<<db->common.d_r_reg);
  return true;
 
}

/**************************************************
 * Helpers for setting the gain
 **************************************************/

static void
_set_gc2(struct db_dbsrx_dummy *db, int gc2){
  db->common.d_gc2 = gc2;
  _send_reg_5(db);
}

/**************************************************
 * Set the gain
 **************************************************/
bool
db_dbsrx_set_gain(struct db_base *dbb, u2_fxpt_gain_t gain){
  struct db_dbsrx_dummy *db = (struct db_dbsrx_dummy *) dbb;
  
  u2_fxpt_gain_t rfgain, bbgain;

  if(!(gain >= db->base.gain_min && gain <= db->base.gain_max)) {
    return false;
  }
  
  if(gain < U2_DOUBLE_TO_FXPT_GAIN(RFGAINMAX)) {
    rfgain = gain;
    bbgain = 0;
  }
  else {
    rfgain = U2_DOUBLE_TO_FXPT_GAIN(RFGAINMAX);
    bbgain = gain - U2_DOUBLE_TO_FXPT_GAIN(RFGAINMAX);
  }

  int rf_gain_slope_q8 = 256 * 4096 * (VMAXGAIN-VMINGAIN) / RFGAINMAX / DACFULLSCALE;
  int rf_gain_offset_q8 = 128 * 256 * 4096 * VMINGAIN / DACFULLSCALE;
  
  int rfdac = (rfgain*rf_gain_slope_q8 + rf_gain_offset_q8)>>15;

  //printf("Set RF Gain %d, %d\n",rfgain,rfdac);
  lsdac_write_rx(1,rfdac);
  
  // Set GC2
  int bb_gain_slope_q8 = 256*(0-31)/(BBGAINMAX-0);

  int gc2 = u2_fxpt_gain_round_to_int((bb_gain_slope_q8 * bbgain)>>8) + 31;
  //printf("Set BB Gain: %d, gc2 %d\n",bbgain,gc2);

  _set_gc2(db, gc2);

  return true;
}

/**************************************************
 * Helpers for setting the bw
 **************************************************/
static void
_set_m(struct db_dbsrx_dummy *db, int m){
  db->common.d_m = m;
  _send_reg_4(db);
}
  
static void
_set_fdac(struct db_dbsrx_dummy *db, int fdac){
  db->common.d_fdac = fdac;
  _send_reg_3(db);
}
