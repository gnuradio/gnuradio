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

#include "db_bitshark_rx.h"
#include <memory_map.h>
#include <db_base.h>
#include <hal_io.h>
#include <mdelay.h>
#include <lsdac.h>
#include <clocks.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <i2c.h>

/* Note: Thie general structure of this file is based on the db_wbxng.c 
   codebase for the wbx daughterboard. */

/* The following defines specify the address map provided by the
   Bitshark USRP Rx (BURX) board. These registers are all accessed over I2C. */
#define RF_CENTER_FREQ_REG 0x00
#define RF_CHAN_FILTER_BW_REG 0x01
#define RF_GAIN_REG 0x02
#define BB_GAIN_REG 0x03
#define ADF4350_REG 0x10
#define SKY73202_REG 0x11
#define CLOCK_SCHEME_REG 0x20

/* The following table lists the registers provided by the Bitshark board 
   that are accessible over I2C:
   --------------------------------------------------------
   |RegAddr: 0x00-RF Center Freq register |
       |4-bytes 0x00|
       |4-byte unsigned RF center freq (in KHz)|
   |RegAddr: 0x01-RF channel filter bandwidth register |
       |4-bytes 0x00|
       |4-byte unsigned RF channel filter bw (in KHz)|
   |RegAddr: 0x02-RF gain register |
       |7-bytes 0x00|
       |1-byte signed RF gain (in dB)|
   |RegAddr: 0x03-Baseband gain register |
       |4-bytes 0x00|
       |4-byte signed baseband filter gain (in dB)|
   |RegAddr: 0x10-ADF4350 register |
       |4-bytes 0x00|
       |4-byte ADF4350 register value (actual ADF4350 reg addr embedded 
        within 4-byte value)|
   |RegAddr: 0x11-SKY73202 register |
       |5-bytes 0x00|
       |1-byte reg 0 of SKY73202 |
       |1-byte reg 1 of SKY73202 |
       |1-byte reg 2 of SKY73202 |
   |RegAddr: 0x20-Clock Scheme |
       |3-bytes 0x00|
       |1-byte indicating clocking scheme:
        -0x00 -> BURX local TCXO off, BURX accepts ref clock from
	         USRP2 (freq of USRP2's ref clock specified in bytes 2-5)
	-0x01 -> BURX local TCXO on, BURX uses its local TCXO as its ref
	         clock, TCXO signal output for use as phase lock for USRP2 |
       |4-byte USRP2 ref clock freq in hz (only needed if byte 1 set to 0x00) |
       
  ---------------------------------------------------------------------------
   
   As an example, lets say the client wants to set an RF center freq of
   1000 MHz.  In KHz, this translates to 1000000 (resolution is only down to
   steps of 1 KHz), which is 0x000F4240 in hex.  So the complete 9-byte I2C 
   sequence that the client should send is as follows:
   byte 0: 0x00-register 0x00 is the target of the write operation
   bytes 1-4: 0x00 (padding)
   byte 5: 0x00 (MSB of the 1000000 KHz value, in hex)
   byte 6: 0x0F
   byte 7: 0x42
   byte 8: 0x40 (LSB of the 1000000 KHz value, in hex)
   
   How about another example...lets say the client wants to setup the clock
   scheme to use scheme #1 where the 26 MHz TCXO on the BURX board is enabled,
   and is provided to the USRP2 for it to phase lock to it as an external ref.  
   26 MHz (i.e. 26 million), in hex, is 0x18CBA80.
   So the complete 9-byte I2C sequence that the client should send is as follows:
   byte 0: 0x20-register 0x20 is the target of the write operation
   bytes 1-3: 0x00 (padding)
   byte 4: 0x01 (indicating that clock scheme #1 is wanted)
   byte 5: 0x01 (MSB of the BURX ref clk freq)
   byte 6: 0x8C
   byte 7: 0xBA
   byte 8: 0x80 (LSB of the BURX ref clk freq)

   Note: The endian-ness of 4-byte values used in I2C cmds is different on 
   USRP2 compared to USRP1.
   
*/

#define NUM_BYTES_IN_I2C_CMD 9
#define I2C_ADDR 0x47

bool bitshark_rx_init(struct db_base *dbb);
bool bitshark_rx_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc);
bool bitshark_rx_set_gain(struct db_base *dbb, u2_fxpt_gain_t gain);
bool bitshark_rx_set_bw(struct db_base *dbb, uint16_t bw);

static bool set_clock_scheme(uint8_t clock_scheme, uint32_t ref_clk_freq);

/*
 * The class instances
 */
struct db_bitshark_rx db_bitshark_rx = {
    .base.dbid = 0x0070,
    .base.is_tx = false,
    .base.output_enables = 0x0000,
    .base.used_pins = 0x0000,
    .base.freq_min = U2_DOUBLE_TO_FXPT_FREQ(300e6),
    .base.freq_max = U2_DOUBLE_TO_FXPT_FREQ(4000e6),
    .base.gain_min = U2_DOUBLE_TO_FXPT_GAIN(0),
    .base.gain_max = U2_DOUBLE_TO_FXPT_GAIN(42),
    .base.gain_step_size = U2_DOUBLE_TO_FXPT_GAIN(6),
    .base.is_quadrature = true,
    .base.i_and_q_swapped = true,
    .base.spectrum_inverted = false,
    .base.default_lo_offset = U2_DOUBLE_TO_FXPT_FREQ(0),
    .base.init = bitshark_rx_init,
    .base.set_freq = bitshark_rx_set_freq,
    .base.set_gain = bitshark_rx_set_gain,
    .base.set_tx_enable = 0,
    .base.atr_mask = 0x0000,
    .base.atr_txval = 0,
    .base.atr_rxval = 0,
    .base.set_antenna = 0,
    .extra.bw_min = 660, /* in KHz, so 660 KHz */
    .extra.bw_max = 56000, /* in KHz, so 56 MHz */
    .extra.set_bw = bitshark_rx_set_bw
};

bool
bitshark_rx_init(struct db_base *dbb)
{
    struct db_bitshark_rx_dummy *db = (struct db_bitshark_rx_dummy *) dbb;    

    clocks_enable_rx_dboard(true, 0);
    /* hal_gpio_write( GPIO_RX_BANK, ENABLE_5|ENABLE_33, ENABLE_5|ENABLE_33 ); */
    /* above isn't needed, since we don't have any GPIO from the FPGA */
    
    /* setup the clock scheme to accept the USRP2's 100 MHz ref clk */
    set_clock_scheme(0,100000000);

    /* initial setting of gain */
    dbb->set_gain(dbb,U2_DOUBLE_TO_FXPT_GAIN(20.0));

    /* Set the freq now to get the one time 10ms delay out of the way. */
    u2_fxpt_freq_t	dc;
    dbb->set_freq(dbb, dbb->freq_min, &dc);

    /* set up the RF bandwidth of the signal of interest...Note: there
       doesn't appear to be a standard way of setting this bandwidth
       in USRP2-land (compared to USRP1-land, where we have the
       straight-forward set_bw() method).  Not sure why this is, but
       for now, simply set the bandwidth once for the intended
       application. */
    db->extra.set_bw(dbb, 25000);  /* 25 MHz channel bw */

    return true;
}

bool
bitshark_rx_set_freq(struct db_base *dbb, u2_fxpt_freq_t freq, u2_fxpt_freq_t *dc)
{
    struct db_bitshark_rx_dummy *db = (struct db_bitshark_rx_dummy *) dbb;    
    unsigned char args[NUM_BYTES_IN_I2C_CMD];
    unsigned char val[4];
    uint32_t freq_in_hz = (uint32_t)(u2_fxpt_freq_round_to_uint(freq));
    uint32_t freq_div_5mhz = freq_in_hz/5000000;
    uint32_t freq_rounded_to_5mhz_in_khz = freq_div_5mhz*5000;
    uint64_t freq_rounded_to_5mhz_in_hz = ((uint64_t)freq_rounded_to_5mhz_in_khz)*1000;
    uint64_t temp;
   
    if(!(freq>=db->base.freq_min && freq<=db->base.freq_max)) 
    {
	return false;
    }

    /* There is a bug in the BURX firmware where tuning to frequencies
       above 2.2 GHz will result in a small final frequency error
       (up to a few KHz).  This bug is due to an overflow of a 16-bit
       value when the input reference clock is sufficiently high (such
       as the 100 MHz clock used on the USRP2), AND the requested tuning
       frequency is not a multiple of 5 MHz.  A fix for the BURX firmware
       is available from Epiq Solutions, but requires an AVR microcontroller 
       programmer to update the firmware on the BURX card directly.  An 
       alternate solution is to enforce a policy where the BURX card only
       tunes to frequencies that are multiples of 5 MHz, and uses the
       DDC in the FPGA to perform any fine-tuning less than 5 MHz.  So
       an application can still request an abribrary RF tuning frequency,
       but the BURX card will be directed to tune to the next lowest
       multiple of 5 MHz, and return the DC-centered freq to the calling
       function to allow the DDC in the FPGA to perform the final 
       down-conversion digitally.  This policy also reduces the overall
       spurious content due to the LO synthesizer, as the Frac-N portion
       of the ADF4350 synthesizer isn't being invoked in modes where
       high spur content would be seen. */
    
    memset(args,0x00,NUM_BYTES_IN_I2C_CMD);
    memcpy(val,&freq_rounded_to_5mhz_in_khz,4);
    args[0] = RF_CENTER_FREQ_REG;
    args[5] = val[3];
    args[6] = val[2];
    args[7] = val[1];
    args[8] = val[0];
    
    i2c_write(I2C_ADDR, args, NUM_BYTES_IN_I2C_CMD);
    /* Add a brief delay after each command.  This only seems to be
       necessary when sending a sequence of commands one after the other.
       This issue appears to be specific to the USRP2, since it isn't
       necessary on the USRP1.  The 5 mS delay is a bit of 
       an emperical compromise: too short (say, 1 mS), and every once
       in a great while a command will still be magically dropped on its
       way out...too long (say, 500 mS) and higher-level apps such as
       usrp2_fft.py seem to choke because the init sequence is taking
       too long.  So 5 mS was tested repeatedly without error, and deemed
       reasonable. Not sure if this is an issue with the I2C master
       code in the microblaze or some place else, and I hate magic
       delays too, but this seems to be stable. */
    mdelay(5);

    /* shift the value up so that it is represented properly in the fixed
       point mode...
    */
    temp = freq_rounded_to_5mhz_in_hz << U2_FPF_RP;


    *dc = (u2_fxpt_freq_t)temp;
    return true;
}

bool
bitshark_rx_set_gain(struct db_base *dbb, u2_fxpt_gain_t gain)
{
    struct db_bitshark_rx_dummy *db = (struct db_bitshark_rx_dummy *) dbb;
    
    unsigned char args[NUM_BYTES_IN_I2C_CMD];
    uint8_t final_gain = (uint8_t)(u2_fxpt_gain_round_to_int(gain));
    
    if(!(gain >= db->base.gain_min && gain <= db->base.gain_max)) 
    {
	return false;
    }
    
    memset(args,0x00,NUM_BYTES_IN_I2C_CMD);
    args[0] = RF_GAIN_REG;
    args[5] = final_gain;

    i2c_write(I2C_ADDR, args, NUM_BYTES_IN_I2C_CMD);
    /* Add a brief delay after each command.  This only seems to be
       necessary when sending a sequence of commands one after the other.
       This issue appears to be specific to the USRP2, since it isn't
       necessary on the USRP1.  The 5 mS delay is a bit of 
       an emperical compromise: too short (say, 1 mS), and every once
       in a great while a command will still be magically dropped on its
       way out...too long (say, 500 mS) and higher-level apps such as
       usrp2_fft.py seem to choke because the init sequence is taking
       too long.  So 5 mS was tested repeatedly without error, and deemed
       reasonable. Not sure if this is an issue with the I2C master
       code in the microblaze or some place else, and I hate magic
       delays too, but this seems to be stable. */
    mdelay(5);

    return true;
}

bool
bitshark_rx_set_bw(struct db_base *dbb, uint16_t bw_in_khz)
{
    struct db_bitshark_rx_dummy *db = (struct db_bitshark_rx_dummy *) dbb;
    unsigned char val[2];
    unsigned char args[NUM_BYTES_IN_I2C_CMD];
    
    if(!(bw_in_khz >= db->extra.bw_min && bw_in_khz <= db->extra.bw_max)) 
    {
	return false;
    }
    
    memset(args,0x00,NUM_BYTES_IN_I2C_CMD);
    memcpy(val,&bw_in_khz,2);
    args[0] = RF_CHAN_FILTER_BW_REG;
    args[5] = val[1];
    args[6] = val[0];

    i2c_write(I2C_ADDR, args, NUM_BYTES_IN_I2C_CMD);
    /* Add a brief delay after each command.  This only seems to be
       necessary when sending a sequence of commands one after the other.
       This issue appears to be specific to the USRP2, since it isn't
       necessary on the USRP1.  The 5 mS delay is a bit of 
       an emperical compromise: too short (say, 1 mS), and every once
       in a great while a command will still be magically dropped on its
       way out...too long (say, 500 mS) and higher-level apps such as
       usrp2_fft.py seem to choke because the init sequence is taking
       too long.  So 5 mS was tested repeatedly without error, and deemed
       reasonable. Not sure if this is an issue with the I2C master
       code in the microblaze or some place else, and I hate magic
       delays too, but this seems to be stable. */
    mdelay(5);

    return true;
}

static bool
set_clock_scheme(uint8_t clock_scheme, uint32_t ref_clk_freq)
{
    /* Set the clock scheme for determining how the BURX
       dboard receives its clock.  For the USRP2, there is really only
       one way of doing this, which is to use the 100 MHz ref clk
       on the USRP2 as its reference.  However, it is possible to
       use the BURX's 26 MHz TCXO as the external reference input to
       the USRP, which would provide phase lock between our oscillator
       and the USRP's 100 MHz oscillator.  And since the BURX board
       provides the ability to warp the oscillator, this may be
       useful to some folks.  Otherwise, the BURX board will always
       just take the 100 MHz reference from the USRP2 as its reference.
    */
    
    unsigned char args[NUM_BYTES_IN_I2C_CMD];
    char val[4];

    if (clock_scheme > 1) 
    {
	return false;
    }

    memcpy(val,&ref_clk_freq,4);
    args[0] = CLOCK_SCHEME_REG;
    args[4] = clock_scheme;
    args[5] = val[3];
    args[6] = val[2];
    args[7] = val[1];
    args[8] = val[0];

    i2c_write(I2C_ADDR, args, NUM_BYTES_IN_I2C_CMD);
    /* Add a brief delay after each command.  This only seems to be
       necessary when sending a sequence of commands one after the other.
       This issue appears to be specific to the USRP2, since it isn't
       necessary on the USRP1.  The 5 mS delay is a bit of 
       an emperical compromise: too short (say, 1 mS), and every once
       in a great while a command will still be magically dropped on its
       way out...too long (say, 500 mS) and higher-level apps such as
       usrp2_fft.py seem to choke because the init sequence is taking
       too long.  So 5 mS was tested repeatedly without error, and deemed
       reasonable. Not sure if this is an issue with the I2C master
       code in the microblaze or some place else, and I hate magic
       delays too, but this seems to be stable. */
    mdelay(5);

    return true;
}

