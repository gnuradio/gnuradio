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

// TX IO Pins
#define HB_PA_OFF      (1 << 15)    // 5GHz PA, 1 = off, 0 = on
#define LB_PA_OFF      (1 << 14)    // 2.4GHz PA, 1 = off, 0 = on
#define ANTSEL_TX1_RX2 (1 << 13)    // 1 = Ant 1 to TX, Ant 2 to RX
#define ANTSEL_TX2_RX1 (1 << 12)    // 1 = Ant 2 to TX, Ant 1 to RX
#define TX_EN          (1 << 11)    // 1 = TX on, 0 = TX off
#define AD9515DIV      (1 << 4)     // 1 = Div  by 3, 0 = Div by 2
#define TX_OE_MASK HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|ANTSEL_TX2_RX1|TX_EN|AD9515DIV
#define TX_SAFE_IO HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|AD9515DIV

#define TUN_FREQ_MIN U2_DOUBLE_TO_FXPT_FREQ(2.4e9)
#define TUN_FREQ_MAX U2_DOUBLE_TO_FXPT_FREQ(6.0e9)

bool xcvr2450_init(struct db_base *db);
bool xcvr2450_set_freq(struct db_base *db, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool xcvr2450_set_gain_rx(struct db_base *db, u2_fxpt_gain_t gain);
bool xcvr2450_set_gain_tx(struct db_base *db, u2_fxpt_gain_t gain);
bool xcvr2450_set_tx_enable(struct db_base *db, bool on);

struct db_xcvr2450_common {
  //int d_spi_format, d_spi_enable;
  
  int d_mimo, d_int_div, d_frac_div, d_highband, d_five_gig;
  int d_cp_current, d_ref_div, d_rssi_hbw;
  int d_txlpf_bw, d_rxlpf_bw, d_rxlpf_fine, d_rxvga_ser;
  int d_rssi_range, d_rssi_mode, d_rssi_mux;
  int d_rx_hp_pin, d_rx_hpf, d_rx_ant;
  int d_tx_ant, d_txvga_ser, d_tx_driver_lin;
  int d_tx_vga_lin, d_tx_upconv_lin, d_tx_bb_gain;
  int d_pabias_delay, d_pabias; //rx_rf_gain, rx_bb_gain;//, d_txgain;
  //int d_rx_rf_gain, d_rx_bb_gain;

  int d_reg_standby, d_reg_int_divider, d_reg_frac_divider, d_reg_bandselpll;
  int d_reg_cal, dsend_reg, d_reg_lpf, d_reg_rxrssi_ctrl, d_reg_txlin_gain;
  int d_reg_pabias;//, d_reg_rxgain;//, d_reg_txgain;

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
  .base.used_pins = RX_SAFE_IO,
  .base.freq_min = TUN_FREQ_MIN,
  .base.freq_max = TUN_FREQ_MAX,
  .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
  .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(92),
  .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(1),
  .base.is_quadrature = true,
  .base.i_and_q_swapped = true,
  .base.spectrum_inverted = false,
  //.base.lo_offset = U2_DOUBLE_TO_FXPT_FREQ(4.25e6),
  .base.init = xcvr2450_init,
  .base.set_freq = xcvr2450_set_freq,
  .base.set_gain = xcvr2450_set_gain_rx,
  .base.set_tx_enable = 0,
  .base.atr_mask = 0x00E0, //CHECK this
  //.base.atr_txval = POWER_UP,
  //.base.atr_rxval = POWER_UP|MIX_EN,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .common.spi_mask = SPI_SS_RX_DB,
};

struct db_xcvr2450_tx db_xcvr2450_tx = {
  .base.dbid = 0x0060,
  .base.is_tx = true,
  .base.output_enables = TX_OE_MASK,
  .base.used_pins = TX_SAFE_IO,
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
  .base.set_tx_enable = xcvr2450_set_tx_enable,
  .base.atr_mask = 0x00E0, //CHECK this
  //.base.atr_txval = POWER_UP|MIX_EN, 
  //.base.atr_rxval = POWER_UP|ANT_SW,
  // .base.atr_tx_delay =
  // .base.atr_rx_delay =
  .common.spi_mask = SPI_SS_TX_DB,
};

/**************************************************
 * Registers
 **************************************************/
void
send_reg(struct db_xcvr2450_dummy *db, int v){
  // Send 24 bits, it keeps last 18 clocked in
  spi_transact(SPI_TXONLY,db->common.spi_mask,v,24,SPIF_PUSH_FALL);
  printf("xcvr2450: Setting reg %d to %06X\n", (v&15), v);
}

void
set_reg_txgain(struct db_xcvr2450_dummy *db, int txgain){
  int reg_txgain = (txgain<<4) | 12;
  send_reg(db, reg_txgain);
}

void
set_reg_rxgain(struct db_xcvr2450_dummy *db, int rx_bb_gain, int rx_rf_gain){
  int reg_rxgain = ( (rx_rf_gain<<9) | (rx_bb_gain<<4) | 11);
  send_reg(db, reg_rxgain);
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
  db->common.d_ref_div = 4;       // 1 to 7
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
  //db->common.d_rx_rf_gain = 0;    // 0 = 0dB, 1 = 0dB, 2 = 15dB, 3 = 30dB
  //db->common.d_rx_bb_gain = 16;   // 0 = min, 31 = max (0 - 62 dB)

  //db->common.d_txgain = 63;       // 0 = min, 63 = max
  
  /*
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
        
  // Initialize chipset
  // TODO: perform reset sequence to ensure power up defaults
  set_reg_standby();
  set_reg_bandselpll();
  set_reg_cal();
  set_reg_lpf();
  set_reg_rxrssi_ctrl();
  set_reg_txlin_gain();
  set_reg_pabias();
  set_reg_rxgain();
  set_reg_txgain();
  //FIXME: set_freq(2.45e9);
  */
  
  db->base.set_gain(dbb, (db->base.gain_max - db->base.gain_min)/2); //set mid-range gain
  return true;
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
  
  
  u2_fxpt_freq_t vco_freq;

  if(freq > U2_DOUBLE_TO_FXPT_FREQ(3e9)) {
    db->common.d_five_gig = 1;
    db->common.d_ref_div = 1;
    db->common.d_ad9515_div = 3;
    //scaler = U2_DOUBLE_TO_FXPT_FREQ(4.0/5.0);
    vco_freq = (4*freq)/5;
  }
  else {
    db->common.d_five_gig = 0;
    db->common.d_ref_div = 1;
    db->common.d_ad9515_div = 3;
    //scaler = U2_DOUBLE_TO_FXPT_FREQ(4.0/3.0);
    vco_freq = (4*freq)/3;
  }

  if(freq > U2_DOUBLE_TO_FXPT_FREQ(5.27e9)) {
    db->common.d_highband = 1;
  }
  else {
    db->common.d_highband = 0;
  }

  //double vco_freq = target_freq*scaler;
  //double sys_clk = usrp()->fpga_master_clock_freq();  // Usually 64e6 
  u2_fxpt_freq_t ref_clk = U2_DOUBLE_TO_FXPT_FREQ(MASTER_CLK_RATE)/db->common.d_ad9515_div;
  u2_fxpt_freq_t phdet_freq = ref_clk/db->common.d_ref_div;
  
  //double div = vco_freq/phdet_freq;
 // d_int_div = int(floor(div));
 // d_frac_div = int((div-d_int_div)*65536.0);
 // double actual_freq = phdet_freq*(d_int_div+(d_frac_div/65536.0))/scaler;
  db->common.d_int_div = vco_freq/phdet_freq;
  *dc = db->common.d_int_div*phdet_freq*freq/vco_freq;
 
  
  printf("RF=%d VCO=%d RefDiv=%d Phdet=%d Div=%d ActualRF=%d\n",
    u2_fxpt_freq_round_to_int(freq), u2_fxpt_freq_round_to_int(vco_freq),
    db->common.d_ref_div, u2_fxpt_freq_round_to_int(phdet_freq),
    db->common.d_int_div, u2_fxpt_freq_round_to_int(*dc));
/*
  set_gpio();
  set_reg_int_divider();
  set_reg_frac_divider();
  set_reg_bandselpll();

  args.ok = lock_detect();
  args.baseband_freq = actual_freq;

  if(args.ok) {
    if((target_freq > 5.275e9) && (target_freq <= 5.35e9)) {
      d_highband = 0;
      set_reg_bandselpll();
      args.ok = lock_detect();
      //printf("swap to 0 at %f, ok %d\n", target_freq, args.ok);
    }
    if((target_freq >= 5.25e9) && (target_freq <= 5.275e9)) {
      d_highband = 1;
      set_reg_bandselpll();
      args.ok = lock_detect();
      //printf("swap to 1 at %f, ok %d\n", target_freq, args.ok);
    }
    if(!args.ok){
      //printf("Fail %f\n", target_freq);
    }
  }*/
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
  int rx_rf_gain = 0, rx_bb_gain = 0;
  // Split the gain between RF and baseband
  // This is experimental, not prescribed
  if(gain < U2_DOUBLE_TO_FXPT_GAIN(30.0)) {
    rx_rf_gain = 0; // 0 dB RF gain
    rx_bb_gain = gain/U2_DOUBLE_TO_FXPT_GAIN(2.0);
  }
  else if(gain >= U2_DOUBLE_TO_FXPT_GAIN(30.0) && gain < U2_DOUBLE_TO_FXPT_GAIN(61.0)) {
    rx_rf_gain = 2; // 15 dB RF gain
    rx_bb_gain = (gain-U2_DOUBLE_TO_FXPT_GAIN(15.0))/U2_DOUBLE_TO_FXPT_GAIN(2.0);
  }
  else if(gain >= U2_DOUBLE_TO_FXPT_GAIN(61.0)) {
    rx_rf_gain = 3; // 30.5 dB RF gain
    rx_bb_gain = (gain-U2_DOUBLE_TO_FXPT_GAIN(30.5))/U2_DOUBLE_TO_FXPT_GAIN(2.0);
  }
  set_reg_rxgain(db, rx_bb_gain, rx_rf_gain);
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
  int txgain = gain/db->base.gain_step_size;
  set_reg_txgain(db, txgain);
  return true;
}

bool
xcvr2450_set_tx_enable(struct db_base *db, bool on){
  return true;
}
