//
// Copyright 2010 Free Software Foundation, Inc.
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
#include "config.h"
#endif

#include <usrp/db_bitshark_rx.h>
#include <db_base_impl.h>
#include <cmath>
#include <cstdio>
#include <string.h>
#include <stdint.h>

/* Note: Thie general structure of this file is based on the db_dbsrx.cc 
   codebase for the dbsrx daughterboard. */

/* The following defines specify the address map provided by the
   Bitshark card. These registers are all accessed over I2C. */
#define RF_CENTER_FREQ_REG 0x00
#define RF_CHAN_FILTER_BW_REG 0x01
#define RF_GAIN_REG 0x02
#define BB_GAIN_REG 0x03
#define ADF4350_REG 0x10
#define SKY73202_REG 0x11
#define CLOCK_SCHEME_REG 0x20

/* The following table lists the registers provided by the BURX board that
   are accessible over I2C:
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
	         USRP (freq of USRP's ref clock specified in bytes 2-5)
	-0x01 -> BURX local TCXO on, BURX uses its local TCXO as its ref
	         clock, TCXO signal output for use by USRP |
       |4-byte USRP ref clock freq in hz (only needed if byte 1 set to 0x00) |
       
  ---------------------------------------------------------------------------
   
   As an example, lets say the client wants to set an RF center freq of
   1000 MHz.  In KHz, this translates to 1000000 (resolution is only down to
   steps of 1 KHz), which is 0x000F4240 in hex.  So the complete 9-byte I2C 
   sequence that the client should send is as follows:
   byte 0: 0x00-register 0x00 is the target of the write operation
   bytes 1-4: 0x00 (padding)
   byte 5: 0x40 (LSB of the 1000000 KHz value, in hex)
   byte 6: 0x42
   byte 7: 0x0F
   byte 8: 0x00 (MSB of the 1000000 KHz value, in hex)

   If using the usrper cmd-line application on a PC, this sequence would
   be sent as follows (assuming that the BURX is in slot A):
   
   # usrper i2c_write 0x47 000000000040420F00
   
   How about another example...lets say the client wants to setup the clock
   scheme to use scheme #1 where the 26 MHz TCXO on the BURX board is enabled,
   and is provided to the USRP.  26 MHz (i.e. 26 million), in hex, is 0x18CBA80.
   So the complete 9-byte I2C sequence that the client should send is as follows:
   byte 0: 0x20-register 0x20 is the target of the write operation
   bytes 1-3: 0x00 (padding)
   byte 4: 0x01 (indicating that clock scheme #1 is wanted)
   byte 5: 0x80 (LSB of the BURX ref clk freq)
   byte 6: 0xBA
   byte 7: 0x8C
   byte 8: 0x01 (MSB of the BURX ref clk freq)
   
   To enable the BURX local ref clk, which will also make it available on the
   on-board U.FL connector as a source for the USRP, a user can also use
   the usrper cmd-line application on a PC.  The following sequence would
   be sent (assuming that the BURX is in slot A):
   
   # usrper i2c_write 0x47 200000000180BA8C01

*/

#define NUM_BYTES_IN_I2C_CMD 9   

/*****************************************************************************/

db_bitshark_rx::db_bitshark_rx(usrp_basic_sptr _usrp, int which)
  : db_base(_usrp, which)
{
    // Control Bitshark receiver USRP daughterboard.
    // 
    // @param usrp: instance of usrp.source_c
    // @param which: which side: 0, 1 corresponding to RX_A or RX_B respectively

    // turn off all outputs
    usrp()->_write_oe(d_which, 0, 0xffff);

    if (which == 0) 
    {
	d_i2c_addr = 0x47;
    }
    else 
    {
	d_i2c_addr = 0x45;
    }

    // initialize gain
    set_gain((gain_min() + gain_max()) / 2.0);

    // by default, assume we're using the USRPs clock as the ref clk,
    // so setup the clock scheme and frequency.  If the user wants
    // to use the Bitshark's TCXO, the clock scheme should be set
    // to 1, the freq should be set to 26000000, and a top-level
    // 'make' and 'make install' needs to be executed.  In addition, 
    // a U.FL to SMA cable needs to connect J6 on the Bitshark to 
    // the external clk input on the USRP
    set_clock_scheme(0,64000000);

    set_bw(8e6); // Default IF bandwidth to match USRP1 max host bandwidth

    bypass_adc_buffers(true);
}

db_bitshark_rx::~db_bitshark_rx()
{
    shutdown();
}

/************ Private Functions **********************/

void
db_bitshark_rx::_set_pga(int pga_gain)
{
    assert(pga_gain>=0 && pga_gain<=20);
    if(d_which == 0) 
    {
	usrp()->set_pga (0, pga_gain);
	usrp()->set_pga (1, pga_gain);
    }
    else 
    {
	usrp()->set_pga (2, pga_gain);
	usrp()->set_pga (3, pga_gain);
    }
}

/************ Public Functions **********************/
void
db_bitshark_rx::shutdown()
{
    if (!d_is_shutdown)
    {
	d_is_shutdown = true;
    }
}

bool
db_bitshark_rx::set_bw (float bw)
{
    std::vector<int> args(NUM_BYTES_IN_I2C_CMD,0);
    uint16_t rf_bw_in_khz = (uint16_t)(bw/1000.0);
    char val[4];
    bool result = false;
    uint8_t try_count = 0;
    
    memset(val,0x00,4);
    if (rf_bw_in_khz < 660  || rf_bw_in_khz > 56000) 
    {
	fprintf(stderr, "db_bitshark_rx::set_bw: bw (=%d) must be between 660 KHz and 56 MHz inclusive\n", rf_bw_in_khz);
	return false;
    }
    //fprintf(stdout,"Setting bw: requested bw in khz is %d\r\n",rf_bw_in_khz);
    memcpy(val,&rf_bw_in_khz,4);
    args[0] = RF_CHAN_FILTER_BW_REG;
    args[5] = val[0];
    args[6] = val[1];
    args[7] = val[2];
    args[8] = val[3];
    while ((result != true) && (try_count < 3))
    {
	result=usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
	try_count++;
    }

    if (result == false)
    {
	fprintf(stderr, "db_bitshark_rx:set_bw: giving up after 3 tries without success\n");
    }
    
    return result;
}

/* The gain referenced below is RF gain only.  There are two independent
   gain settings at RF: a digital step attenuator (providing 0, -6, -12, and
   -18 dB of attenuation), and a second LNA (LNA2) that provides ~25 dB of
   gain (roughly...it actually depends on the RF freq).  So combining these
   two stages can provide an overall gain range from 0 (which is mapped
   to -18 dB on the step attenuator + LNA2 turned off) to 42 (which is
   mapped to 0 dB on the step attenuator + LNA2 turned on).  
   
   There could be better ways to map these, but this is sufficient for
   now. */
float
db_bitshark_rx::gain_min()
{
    return 0;
}

float
db_bitshark_rx::gain_max()
{
    return 42;
}

float
db_bitshark_rx::gain_db_per_step()
{
    return 6;
}

bool 
db_bitshark_rx::set_gain(float gain)
{
    // Set the gain.
    // 
    // @param gain:  RF gain in decibels, range of 0-42
    // @returns True/False
    
    std::vector<int> args(NUM_BYTES_IN_I2C_CMD,0);
    bool result = false;
    uint8_t try_count = 0;
        
    if (gain < gain_min() || gain > gain_max()) 
    {
	fprintf(stderr,"db_bitshark_rx::set_gain: gain (=%f) must be between %f and %f inclusive\n", gain,gain_min(),gain_max());
	return false;
    }
    //fprintf(stdout,"db_bitshark_rx::set_gain: requested gain of %f\r\n",gain);
    args[0] = RF_GAIN_REG;
    args[5] = (int)gain;

    while ((result != true) && (try_count < 3))
    {
	result=usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
	try_count++;
    }

    if (result == false)
    {
	fprintf(stderr, "db_bitshark_rx:set_gain: giving up after 3 tries without success\n");
    }
    
    return result;
}


bool 
db_bitshark_rx::set_clock_scheme(uint8_t clock_scheme, uint32_t ref_clk_freq)
{
    // Set the clock scheme for determining how the BURX
    // dboard receives its clock.  Note: Ideally, the constructor for the
    // BURX board could simply call this method to set how it wants the
    // clock scheme configured.  However, depending on the application
    // using the daughterboard, the constructor may run _after_ some
    // other portion of the application needs the FPGA.  And if the
    // the clock source for the FPGA was the BURX's 26 MHz TCXO, we're in
    // a chicken-before-the-egg dilemna.  So the solution is to leave
    // this function here for reference in case an app wants to use it,
    // and also give the user the ability to set the clock scheme through
    // the usrper cmd-line application (see example at the top of this
    // file).
    // 
    // @param clock_scheme
    // @param ref_clk_freq in Hz
    // @returns True/False
    
    std::vector<int> args(NUM_BYTES_IN_I2C_CMD,0);
    bool result = false;
    uint8_t try_count = 0;
    char val[4];
        
    if (clock_scheme > 1) 
    {
	fprintf(stderr,"db_bitshark_rx::set_clock_scheme: invalid scheme %d\n",clock_scheme);
	return false;
    }
    //fprintf(stdout,"db_bitshark_rx::set_clock_scheme: requested clock schem of %d with freq %d Hz \n",clock_scheme,ref_clk_freq);
    memcpy(val,&ref_clk_freq,4);
    args[0] = CLOCK_SCHEME_REG;
    args[4] = (int)clock_scheme;
    args[5] = val[0];
    args[6] = val[1];
    args[7] = val[2];
    args[8] = val[3];

    while ((result != true) && (try_count < 3))
    {
	result=usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
	try_count++;
    }

    if (result == false)
    {
	fprintf(stderr, "db_bitshark_rx:set_clock_scheme: giving up after 3 tries without success\n");
    }
    return result;
}

double
db_bitshark_rx::freq_min()
{    
    return 300e6;
}

double
db_bitshark_rx::freq_max()
{    
    return 4e9;
}

struct freq_result_t
db_bitshark_rx::set_freq(double freq)
{
    // Set the frequency.
    // 
    // @param freq:  target RF frequency in Hz
    // @type freq:   double
    // 
    // @returns (ok, actual_baseband_freq) where:
    //   ok is True or False and indicates success or failure,
    //   actual_baseband_freq is RF frequency that corresponds to DC in the IF.
    
    std::vector<int> args(NUM_BYTES_IN_I2C_CMD,0);
    std::vector<int> bytes(2);
    char val[4];
    freq_result_t act_freq = {false, 0};
    uint32_t freq_in_khz = (uint32_t)(freq/1000.0);
    bool result = false;
    uint8_t try_count = 0;
        
    memset(val,0x00,4);
    if(!(freq>=freq_min() && freq<=freq_max())) 
    {
	return act_freq;
    }
    
    //fprintf(stdout,"db_bitshark_rx::set_freq: requested freq is %d KHz\n",freq_in_khz);
    memcpy(val,&freq_in_khz,4);
    args[0] = RF_CENTER_FREQ_REG;
    args[5] = val[0];
    args[6] = val[1];
    args[7] = val[2];
    args[8] = val[3];

    while ((result != true) && (try_count < 3))
    {
	result=usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
	try_count++;
    }

    if (result == false)
    {
	fprintf(stderr, "db_bitshark_rx:set_freq: giving up after 3 tries without success\n");
    }
        
    act_freq.ok = result;
    act_freq.baseband_freq = (double)freq;
    return act_freq;
}

bool 
db_bitshark_rx::is_quadrature()
{    
    // Return True if this board requires both I & Q analog channels.  
    return true;
}

bool
db_bitshark_rx::i_and_q_swapped()
{
    // Returns True since our I and Q channels are swapped
    return true;
}
