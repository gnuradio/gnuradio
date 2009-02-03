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

#include <memory_map.h>
#include <db_base.h>
#include <stdio.h>
#include <spi.h>
#include <hal_io.h>
#include <clocks.h>

void set_atr_regs(int bank, struct db_base *db); //FIXME I need to be in a header

// RX IO Pins
#define LOCKDET (1 << 15)           // This is an INPUT!!!
#define EN      (1 << 14)
#define RX_EN   (1 << 13)           // 1 = RX on, 0 = RX off
#define RX_HP   (1 << 12)
#define B1      (1 << 11)
#define B2      (1 << 10)
#define B3      (1 << 9)
#define B4      (1 << 8)
#define B5      (1 << 7)
#define B6      (1 << 6)
#define B7      (1 << 5)
#define RX_OE_MASK EN|RX_EN|RX_HP|B1|B2|B3|B4|B5|B6|B7
#define RX_SAFE_IO EN
#define RX_ATR_MASK EN|RX_EN|RX_HP

// TX IO Pins
#define HB_PA_OFF      (1 << 15)    // 5GHz PA, 1 = off, 0 = on
#define LB_PA_OFF      (1 << 14)    // 2.4GHz PA, 1 = off, 0 = on
#define ANTSEL_TX1_RX2 (1 << 13)    // 1 = Ant 1 to TX, Ant 2 to RX
#define ANTSEL_TX2_RX1 (1 << 12)    // 1 = Ant 2 to TX, Ant 1 to RX
#define TX_EN          (1 << 11)    // 1 = TX on, 0 = TX off
#define AD9515DIV      (1 << 4)     // 1 = Div  by 3, 0 = Div by 2
#define TX_OE_MASK HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|ANTSEL_TX2_RX1|TX_EN|AD9515DIV
#define TX_SAFE_IO HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|AD9515DIV
#define TX_ATR_MASK HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|ANTSEL_TX2_RX1|TX_EN|AD9515DIV

#define TUN_FREQ_MIN U2_DOUBLE_TO_FXPT_FREQ(2.4e9)
#define TUN_FREQ_MAX U2_DOUBLE_TO_FXPT_FREQ(6.0e9)

bool xcvr2450_init(struct db_base *db);
bool xcvr2450_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool xcvr2450_set_gain_rx(struct db_base *db, u2_fxpt_gain_t gain);
bool xcvr2450_set_gain_tx(struct db_base *db, u2_fxpt_gain_t gain);
bool xcvr2450_set_tx_enable(struct db_base *db, bool on);

struct db_xcvr2450_common {
  int d_mimo, d_int_div, d_frac_div, d_highband, d_five_gig;
  int d_cp_current, d_ref_div, d_rssi_hbw;
  int d_txlpf_bw, d_rxlpf_bw, d_rxlpf_fine, d_rxvga_ser;
  int d_rssi_range, d_rssi_mode, d_rssi_mux;
  int d_rx_hp_pin, d_rx_hpf, d_rx_ant;
  int d_tx_ant, d_txvga_ser, d_tx_driver_lin;
  int d_tx_vga_lin, d_tx_upconv_lin, d_tx_bb_gain;
  int d_pabias_delay, d_pabias;
  int d_rx_rf_gain, d_rx_bb_gain, d_txgain;
  int d_ad9515_div;
  int spi_mask;
};

struct db_xcvr2450_dummy {
  struct db_base base;
  struct db_xcvr2450_common common;
};

struct db_xcvr2450_rx {
  struct db_base base;
  struct db_xcvr2450_common common;
};

struct db_xcvr2450_tx {
  struct db_base base;
  struct db_xcvr2450_common common;
};

/*
 * The class instances
 */
struct db_xcvr2450_rx db_xcvr2450_rx = {
  .base.dbid = 0x0061,
  .base.is_tx = false,
  .base.output_enables = RX_OE_MASK,
  .base.used_pins = 0xFFFF,
  .base.freq_min = TUN_FREQ_MIN,
  .base.freq_max = TUN_FREQ_MAX,
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(92),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = false,
  .base.spectrum_inverted = false,
  //.base.lo_offset = U2_DOUBLE_TO_FXPT_FREQ(4.25e6),
  .base.init = xcvr2450_init,
  .base.set_freq = xcvr2450_set_freq,
  .base.set_gain = xcvr2450_set_gain_rx,
  //.base.set_tx_enable = 0,
  .base.atr_mask = RX_ATR_MASK,
  .base.atr_txval = 0x0,
  .base.atr_rxval = 0x0,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .common.spi_mask = SPI_SS_RX_DB,
};

struct db_xcvr2450_tx db_xcvr2450_tx = {
  .base.dbid = 0x0060,
  .base.is_tx = true,
  .base.output_enables = TX_OE_MASK,
  .base.used_pins = 0xFFFF,
  .base.freq_min = TUN_FREQ_MIN,
  .base.freq_max = TUN_FREQ_MAX,
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(30),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(30.0/63.0),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  //.base.lo_offset = U2_DOUBLE_TO_FXPT_FREQ(4.25e6),
  .base.init = xcvr2450_init,
  .base.set_freq = xcvr2450_set_freq,
  .base.set_gain = xcvr2450_set_gain_tx,
  //.base.set_tx_enable = xcvr2450_set_tx_enable,
  .base.atr_mask = TX_ATR_MASK,
  .base.atr_txval = 0x0,
  .base.atr_rxval = 0x0,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .common.spi_mask = SPI_SS_TX_DB,
};

/**************************************************
 * Set Registers
 **************************************************/
void
send_reg(struct db_xcvr2450_dummy *db, int v){
  // Send 24 bits, it keeps last 18 clocked in
  spi_transact(SPI_TXONLY,db->common.spi_mask,v,24,SPIF_PUSH_FALL);
  printf("xcvr2450: Setting reg %d to %x\n", (v&15), v);
}

void
set_reg_standby(struct db_xcvr2450_dummy *db){
  int reg_standby = (
    (db->common.d_mimo<<17) |
    (1<<16)                 |
    (1<<6)                  |
    (1<<5)                  |
    (1<<4)                  | 2);
  send_reg(db, reg_standby);
}

void
set_reg_int_divider(struct db_xcvr2450_dummy *db){
  int reg_int_divider = ((
    (db->common.d_frac_div & 0x03)<<16) |
    (db->common.d_int_div<<4)           | 3);
  send_reg(db, reg_int_divider);
}

void
set_reg_frac_divider(struct db_xcvr2450_dummy *db){
  int reg_frac_divider = ((db->common.d_frac_div & 0xfffc)<<2) | 4;
  send_reg(db, reg_frac_divider);
}

void
set_reg_bandselpll(struct db_xcvr2450_dummy *db){
  int reg_bandselpll = ((db->common.d_mimo<<17) |
    (1<<16) |
    (1<<15) |
    (0<<11) |
    (db->common.d_highband<<10)  |
    (db->common.d_cp_current<<9) |
    (db->common.d_ref_div<<5)    |
    (db->common.d_five_gig<<4)   | 5);
  send_reg(db, reg_bandselpll);
  reg_bandselpll = ((db->common.d_mimo<<17) |
    (1<<16) |
    (1<<15) |
    (1<<11) |
    (db->common.d_highband<<10)  |
    (db->common.d_cp_current<<9) |
    (db->common.d_ref_div<<5)    |
    (db->common.d_five_gig<<4)   | 5);
  send_reg(db, reg_bandselpll);
}

void
set_reg_cal(struct db_xcvr2450_dummy *db){
  // FIXME do calibration
  int reg_cal = (
    (1<<14) | 6);
  send_reg(db, reg_cal);
}

void
set_reg_lpf(struct db_xcvr2450_dummy *db){
  int reg_lpf = (
    (db->common.d_rssi_hbw<<15)  |
    (db->common.d_txlpf_bw<<10)  |
    (db->common.d_rxlpf_bw<<9)   |
    (db->common.d_rxlpf_fine<<4) | 7);
  send_reg(db, reg_lpf);
}

void
set_reg_rxrssi_ctrl(struct db_xcvr2450_dummy *db){
  int reg_rxrssi_ctrl = (
       (db->common.d_rxvga_ser<<16)  |
       (db->common.d_rssi_range<<15) |
       (db->common.d_rssi_mode<<14)  |
       (db->common.d_rssi_mux<<12)   |
       (1<<9)                        |
       (db->common.d_rx_hpf<<6)      |
       (1<<4)                        | 8);
  send_reg(db, reg_rxrssi_ctrl);
}

void
set_reg_txlin_gain(struct db_xcvr2450_dummy *db){
  int reg_txlin_gain = (
      (db->common.d_txvga_ser<<14)     |
      (db->common.d_tx_driver_lin<<12) |
      (db->common.d_tx_vga_lin<<10)    |
      (db->common.d_tx_upconv_lin<<6)  |
      (db->common.d_tx_bb_gain<<4)     | 9);
  send_reg(db, reg_txlin_gain);
}

void
set_reg_pabias(struct db_xcvr2450_dummy *db){
  int reg_pabias = (
      (db->common.d_pabias_delay<<10) |
      (db->common.d_pabias<<4)        | 10);
  send_reg(db, reg_pabias);
}

void
set_reg_rxgain(struct db_xcvr2450_dummy *db){
  int reg_rxgain = (
    (db->common.d_rx_rf_gain<<9) |
    (db->common.d_rx_bb_gain<<4) | 11);
  send_reg(db, reg_rxgain);
}

void
set_reg_txgain(struct db_xcvr2450_dummy *db){
  int reg_txgain = (
    (db->common.d_txgain<<4) | 12);
  send_reg(db, reg_txgain);
}

/**************************************************
 * GPIO
 **************************************************/
void
set_gpio(struct db_xcvr2450_dummy *db){
  // We calculate four values:
  //
  // io_rx_while_rx: what to drive onto io_rx_* when receiving
  // io_rx_while_tx: what to drive onto io_rx_* when transmitting
  // io_tx_while_rx: what to drive onto io_tx_* when receiving
  // io_tx_while_tx: what to drive onto io_tx_* when transmitting
  //
  // B1-B7 is ignored as gain is set serially for now.

  int rx_hp, tx_antsel, rx_antsel, tx_pa_sel;
  if(db->common.d_rx_hp_pin)
    rx_hp = RX_HP;
  else
    rx_hp = 0;

  if(db->common.d_tx_ant)
    tx_antsel = ANTSEL_TX2_RX1;
  else
    tx_antsel = ANTSEL_TX1_RX2;

  if(db->common.d_rx_ant)
    rx_antsel = ANTSEL_TX2_RX1;
  else
    rx_antsel = ANTSEL_TX1_RX2;

  if(db->common.d_five_gig)
    tx_pa_sel = LB_PA_OFF;
  else
    tx_pa_sel = HB_PA_OFF;

  /* FIXME better way to set rx and tx val for RX and TX banks */
  /* set rx bank */
  db->base.atr_rxval = EN|rx_hp|RX_EN;
  db->base.atr_txval = EN|rx_hp;
  set_atr_regs(GPIO_RX_BANK, (struct db_base *)db);
  /* set tx bank */
  db->base.atr_rxval = HB_PA_OFF|LB_PA_OFF|rx_antsel|AD9515DIV;
  db->base.atr_txval = tx_pa_sel|tx_antsel|TX_EN|AD9515DIV;
  set_atr_regs(GPIO_TX_BANK, (struct db_base *)db);
}

/**************************************************
 * Init for TX and RX
 **************************************************/
bool
xcvr2450_init(struct db_base *dbb){
  struct db_xcvr2450_dummy *db = (struct db_xcvr2450_dummy *) dbb;
  // Sane defaults
  db->common.d_mimo = 1;          // 0 = OFF, 1 = ON
  db->common.d_int_div = 192;     // 128 = min, 255 = max
  db->common.d_frac_div = 0;      // 0 = min, 65535 = max
  db->common.d_highband = 0;      // 0 = freq <= 5.4e9, 1 = freq > 5.4e9
  db->common.d_five_gig = 0;      // 0 = freq <= 3.e9, 1 = freq > 3e9
  db->common.d_cp_current = 0;    // 0 = 2mA, 1 = 4mA
  db->common.d_ref_div = 1;       // 1 to 7
  db->common.d_rssi_hbw = 0;      // 0 = 2 MHz, 1 = 6 MHz
  db->common.d_txlpf_bw = 1;      // 1 = 12 MHz, 2 = 18 MHz, 3 = 24 MHz
  db->common.d_rxlpf_bw = 1;      // 0 = 7.5 MHz, 1 = 9.5 MHz, 2 = 14 MHz, 3 = 18 MHz
  db->common.d_rxlpf_fine = 2;    // 0 = 90%, 1 = 95%, 2 = 100%, 3 = 105%, 4 = 110%
  db->common.d_rxvga_ser = 1;     // 0 = RXVGA controlled by B7:1, 1=controlled serially
  db->common.d_rssi_range = 1;    // 0 = low range (datasheet typo), 1=high range (0.5V - 2.0V)
  db->common.d_rssi_mode = 1;     // 0 = enable follows RXHP, 1 = enabled
  db->common.d_rssi_mux = 0;      // 0 = RSSI, 1 = TEMP
  db->common.d_rx_hp_pin = 0;     // 0 = Fc set by rx_hpf, 1 = 600 KHz
  db->common.d_rx_hpf = 0;        // 0 = 100Hz, 1 = 30KHz
  db->common.d_rx_ant = 0;        // 0 = Ant. #1, 1 = Ant. #2
  db->common.d_tx_ant = 0;        // 0 = Ant. #1, 1 = Ant. #2
  db->common.d_txvga_ser = 1;     // 0 = TXVGA controlled by B6:1, 1=controlled serially
  db->common.d_tx_driver_lin = 2; // 0=50% (worst linearity), 1=63%, 2=78%, 3=100% (best lin)
  db->common.d_tx_vga_lin = 2;    // 0=50% (worst linearity), 1=63%, 2=78%, 3=100% (best lin)
  db->common.d_tx_upconv_lin = 2; // 0=50% (worst linearity), 1=63%, 2=78%, 3=100% (best lin)
  db->common.d_tx_bb_gain = 3;    // 0=maxgain-5dB, 1=max-3dB, 2=max-1.5dB, 3=max
  db->common.d_pabias_delay = 15; // 0 = 0, 15 = 7uS
  db->common.d_pabias = 0;        // 0 = 0 uA, 63 = 315uA
  db->common.d_rx_rf_gain = 0;    // 0 = 0dB, 1 = 0dB, 2 = 15dB, 3 = 30dB
  db->common.d_rx_bb_gain = 16;   // 0 = min, 31 = max (0 - 62 dB)
  db->common.d_txgain = 63;       // 0 = min, 63 = max

  clocks_enable_tx_dboard(true, 1);

  /*--------------------------------> !!! taken care of by db base operations
  // Initialize GPIO and ATR
  tx_write_io(TX_SAFE_IO, TX_OE_MASK);
  tx_write_oe(TX_OE_MASK, ~0);
  tx_set_atr_txval(TX_SAFE_IO);
  tx_set_atr_rxval(TX_SAFE_IO);
  tx_set_atr_mask(TX_OE_MASK);

  rx_write_io(RX_SAFE_IO, RX_OE_MASK);
  rx_write_oe(RX_OE_MASK, ~0);
  rx_set_atr_rxval(RX_SAFE_IO);
  rx_set_atr_txval(RX_SAFE_IO);
  rx_set_atr_mask(RX_OE_MASK);
  ---------------------------------------> replaced with set_gpio
  * */

  set_gpio(db);

  // Initialize chipset
  // TODO: perform reset sequence to ensure power up defaults
  set_reg_standby(db);
  set_reg_bandselpll(db);
  set_reg_cal(db);
  set_reg_lpf(db);
  set_reg_rxrssi_ctrl(db);
  set_reg_txlin_gain(db);
  set_reg_pabias(db);
  set_reg_rxgain(db);
  set_reg_txgain(db);
  //FIXME: set_freq(2.45e9);
  u2_fxpt_freq_t dc;
  db->base.set_freq(dbb, U2_DOUBLE_TO_FXPT_FREQ(4.55e9), &dc);

  db->base.set_gain(dbb, (db->base.gain_max - db->base.gain_min)/2); //set mid-range gain
  return true;
}

/**************************************************
 * Lock detect
 **************************************************/
bool
lock_detect(){
  /*
    @returns: the value of the VCO/PLL lock detect bit.
    @rtype: 0 or 1
  */
  if(hal_gpio_read(GPIO_RX_BANK) & LOCKDET) {
    return true;
  }
  else {      // Give it a second chance
    if(hal_gpio_read(GPIO_RX_BANK) & LOCKDET)
      return true;
    else
      return false;
  }
}

/**************************************************
 * Set the freq
 **************************************************/
bool
xcvr2450_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc){
  struct db_xcvr2450_dummy *db = (struct db_xcvr2450_dummy *) dbb;
  //ensure gain in within range
  if(!(freq >= db->base.freq_min && freq <= db->base.freq_max)) {
    return false;
  }

  int scaler;
  if(freq > U2_DOUBLE_TO_FXPT_FREQ(3e9)) {
    db->common.d_five_gig = 1;
    db->common.d_ad9515_div = 3;
    //scaler = (4.0/5.0);
    //vco_freq = (4*freq)/5;
    scaler = 5;
    printf("5-GHZ\n");
  } else {
    db->common.d_five_gig = 0;
    db->common.d_ad9515_div = 3;
    //scaler = (4.0/3.0);
    //vco_freq = (4*freq)/3;
    scaler = 3;
    printf("2.4-GHZ\n");
  }

  if(freq > U2_DOUBLE_TO_FXPT_FREQ(5.408e9)) {
    db->common.d_highband = 1;
    printf("5-HB\n");
  }
  else {
    db->common.d_highband = 0;
    printf("5-LB\n");
  }

  int div_factor = db->common.d_ref_div*db->common.d_ad9515_div*4;

  u2_fxpt_freq_t div_q20 = freq*div_factor/(scaler*MASTER_CLK_RATE);

  db->common.d_int_div = div_q20>>U2_FPF_RP;
  db->common.d_frac_div = (div_q20>>4)&0xffff;

  unsigned int actual_div_q16 = (db->common.d_int_div<<16)+db->common.d_frac_div;

  printf("scaler %d, div_factor %d, %u\n", scaler, div_factor, db->common.d_int_div);

  u2_fxpt_freq_t actual_freq = ((((u2_fxpt_freq_t)MASTER_CLK_RATE)*actual_div_q16*scaler) / div_factor) << 4;
  *dc = actual_freq;

  printf("actual div %u, Target Freq %uKHz, Actual Freq %uKHz\n",
    actual_div_q16,
    u2_fxpt_freq_round_to_int(freq/1000),
    u2_fxpt_freq_round_to_int(actual_freq/1000));

  set_gpio(db);
  set_reg_int_divider(db);
  set_reg_frac_divider(db);
  set_reg_bandselpll(db);

  bool ok = lock_detect();
  if(!ok){
    printf("Fail %uKHz\n", u2_fxpt_freq_round_to_int(freq/1000));
  }
  //  return ok;
  return true;
}

/**************************************************
 * Set RX Gain
 **************************************************/
bool
xcvr2450_set_gain_rx(struct db_base *dbb, u2_fxpt_gain_t gain){
  struct db_xcvr2450_dummy *db = (struct db_xcvr2450_dummy *) dbb;
  //ensure gain in within range
  if(!(gain >= db->base.gain_min && gain <= db->base.gain_max)) {
    return false;
  }
  // Split the gain between RF and baseband
  // This is experimental, not prescribed
  if(gain < U2_DOUBLE_TO_FXPT_GAIN(30.0)) {
    db->common.d_rx_rf_gain = 0; // 0 dB RF gain
    db->common.d_rx_bb_gain = u2_fxpt_gain_round_to_int(gain/2);
  }
  else if(gain >= U2_DOUBLE_TO_FXPT_GAIN(30.0) && gain < U2_DOUBLE_TO_FXPT_GAIN(61.0)) {
    db->common.d_rx_rf_gain = 2; // 15 dB RF gain
    db->common.d_rx_bb_gain = u2_fxpt_gain_round_to_int((gain-U2_DOUBLE_TO_FXPT_GAIN(15.0))/2);
  }
  else if(gain >= U2_DOUBLE_TO_FXPT_GAIN(61.0)) {
    db->common.d_rx_rf_gain = 3; // 30.5 dB RF gain
    db->common.d_rx_bb_gain = u2_fxpt_gain_round_to_int((gain-U2_DOUBLE_TO_FXPT_GAIN(30.5))/2);
  }
  set_reg_rxgain(db);
  return true;
}

/**************************************************
 * Set TX Gain
 **************************************************/
bool
xcvr2450_set_gain_tx(struct db_base *dbb, u2_fxpt_gain_t gain){
  struct db_xcvr2450_dummy *db = (struct db_xcvr2450_dummy *) dbb;
  //ensure gain in within range
  if(!(gain >= db->base.gain_min && gain <= db->base.gain_max)) {
    return false;
  }
  //scale for register and set
  db->common.d_txgain = gain/db->base.gain_step_size;
  set_reg_txgain(db);
  return true;
}
