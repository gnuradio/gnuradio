/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include "ethernet.h"
#include "memory_map.h"
#include "eth_phy.h"
#include "eth_mac.h"
#include "eth_mac_regs.h"
#include "pic.h"
#include "hal_io.h"
#include "nonstdio.h"
#include "bool.h"
#include "i2c.h"
#include "usrp2_i2c_addr.h"


#define VERBOSE 0

static ethernet_t ed_state;
static ethernet_link_changed_callback_t ed_callback = 0;

void 
ethernet_register_link_changed_callback(ethernet_link_changed_callback_t new_callback)
{
  ed_callback = new_callback;
}


static void
ed_set_mac_speed(int speed)
{
  printf("Speed set to %d\n",speed);
  /*
  switch(speed){
  case 10:
    eth_mac->speed = 1;
    break;
  case 100:
    eth_mac->speed = 2;
    break;
  case 1000:
    eth_mac->speed = 4;
    break;
  default:
    break;
  }
  */
}

static void
ed_link_up(int speed)
{
  // putstr("ed_link_up: "); puthex16_nl(speed);

  ed_set_mac_speed(speed);

  if (ed_callback)		// fire link changed callback
    (*ed_callback)(speed);
}

static void
ed_link_down(void)
{
  // putstr("ed_link_down\n");

  if (ed_callback)		// fire link changed callback
    (*ed_callback)(0);
}


static void
ed_link_speed_change(int speed)
{
  ed_link_down();
  ed_link_up(speed);
}

static void
print_flow_control(int flow_control)
{
  static const char *flow_control_msg[4] = {
    "NONE", "WE_TX", "WE_RX", "SYMMETRIC"
  };
  putstr("ethernet flow control: ");
  puts(flow_control_msg[flow_control & 0x3]);
}

static void
check_flow_control_resolution(void)
{
  static const unsigned char table[16] = {
    // index = {local_asm, local_pause, partner_asm, partner_pause}
    FC_NONE,  FC_NONE,  FC_NONE,  FC_NONE,
    FC_NONE,  FC_SYMM,  FC_NONE,  FC_SYMM,
    FC_NONE,  FC_NONE,  FC_NONE,  FC_WE_TX,
    FC_NONE,  FC_SYMM,  FC_WE_RX, FC_SYMM
  };

  int us = eth_mac_miim_read(PHY_AUTONEG_ADV);
  int lp = eth_mac_miim_read(PHY_LP_ABILITY);
  int index = (((us >> 10) & 0x3) << 2) | ((lp >> 10) & 0x3);
  ed_state.flow_control = table[index];

  if (1)
    print_flow_control(ed_state.flow_control);
}

/*
 * Read the PHY state register to determine link state and speed
 */
static void
ed_check_phy_state(void)
{
  int lansr = eth_mac_miim_read(PHY_LINK_AN);
  eth_link_state_t new_state = LS_UNKNOWN;
  int new_speed = S_UNKNOWN;

  if (VERBOSE){
    putstr("LANSR: ");
    puthex16_nl(lansr);
  }

  if (lansr & LANSR_LINK_GOOD){		// link's up
    if (VERBOSE)
      puts("  LINK_GOOD");

    new_state = LS_UP;
    switch (lansr & LANSR_SPEED_MASK){
    case LANSR_SPEED_10:
      new_speed = 10;
      break;
      
    case LANSR_SPEED_100:
      new_speed = 100;
      break;
      
    case LANSR_SPEED_1000:
      new_speed = 1000;
      break;

    default:
      new_speed = S_UNKNOWN;
      break;
    }

    check_flow_control_resolution();
  }
  else {				// link's down
    if (VERBOSE)
      puts("  NOT LINK_GOOD");
    
    new_state = LS_DOWN;
    new_speed = S_UNKNOWN;
  }

  if (new_state != ed_state.link_state){
    ed_state.link_state = new_state;		// remember new state
    if (new_state == LS_UP)
      ed_link_up(new_speed);
    else if (new_state == LS_DOWN)
      ed_link_down();
  }
  else if (new_state == LS_UP && new_speed != ed_state.link_speed){
    ed_state.link_speed = new_speed;		// remember new speed
    ed_link_speed_change(new_speed);
  }
}

/*
 * This is fired when the ethernet PHY state changes
 */
static void
eth_phy_irq_handler(unsigned irq)
{
  ed_check_phy_state();
  eth_mac_miim_write(PHY_INT_CLEAR, ~0);	// clear all ints
}

void
ethernet_init(void)
{
  eth_mac_init(ethernet_mac_addr());

  ed_state.link_state = LS_UNKNOWN;
  ed_state.link_speed = S_UNKNOWN;

  // initialize MAC registers
  //  eth_mac->tx_hwmark = 0x1e;
  //eth_mac->tx_lwmark = 0x19;

  //eth_mac->crc_chk_en = 1;
  //eth_mac->rx_max_length = 2048;

  // configure PAUSE frame stuff
  //eth_mac->tx_pause_en = 1;		// pay attn to pause frames sent to us

  //eth_mac->pause_quanta_set = 38;	// a bit more than 1 max frame 16kb/512 + fudge
  //eth_mac->pause_frame_send_en = 1;	// enable sending pause frames


  // setup PHY to interrupt on changes

  unsigned mask =
    (PHY_INT_AN_CMPL		// auto-neg completed
     | PHY_INT_NO_LINK		// no link after auto-neg
     | PHY_INT_NO_HCD		// no highest common denominator
     | PHY_INT_MAS_SLA_ERR	// couldn't resolve master/slave 
     | PHY_INT_PRL_DET_FLT	// parallel detection fault
     | PHY_INT_LNK_CNG		// link established or broken
     | PHY_INT_SPD_CNG		// speed changed
     );

  eth_mac_miim_write(PHY_INT_CLEAR, ~0);	// clear all pending interrupts
  eth_mac_miim_write(PHY_INT_MASK, mask);	// enable the ones we want

  pic_register_handler(IRQ_PHY, eth_phy_irq_handler);

  // Advertise our flow control configuation.
  //
  // We and the link partner each specify two bits in the base page
  // related to autoconfiguration: NWAY_AR_PAUSE and NWAY_AR_ASM_DIR.
  // The bits say what a device is "willing" to do, not what may actually
  // happen as a result of the negotiation.  There are 4 cases:
  //
  // PAUSE  ASM_DIR
  //
  //  0        0        I have no flow control capability.
  //
  //  1        0        I both assert and respond to flow control.
  //
  //  0        1        I assert flow control, but cannot respond.  That is,
  //                    I want to be able to send PAUSE frames, but will ignore any
  //		 	you send to me.  (This is our configuration.)
  //
  //  1        1        I can both assert and respond to flow control AND I am willing
  //                    to operate symmetrically OR asymmetrically in EITHER direction.
  //                    (We hope the link partner advertises this, otherwise we don't
  //			get what we want.)

  int t = eth_mac_miim_read(PHY_AUTONEG_ADV);
  t &= ~(NWAY_AR_PAUSE | NWAY_AR_ASM_DIR);
  t |= NWAY_AR_ASM_DIR;

  // Say we can't to 10BASE-T or 100BASE-TX, half or full duplex
  t &= ~(NWAY_AR_10T_HD_CAPS | NWAY_AR_10T_FD_CAPS | NWAY_AR_100TX_HD_CAPS | NWAY_AR_100TX_FD_CAPS);

  eth_mac_miim_write(PHY_AUTONEG_ADV, t);
  int r = eth_mac_miim_read(PHY_AUTONEG_ADV);  		// DEBUG, read back
  if (t != r){
    printf("PHY_AUTONEG_ADV: wrote 0x%x, got 0x%x\n", t, r);
  }

  // Restart autonegotation.  
  // We want to ensure that we're advertising our PAUSE capabilities.
  t = eth_mac_miim_read(PHY_CTRL);
  eth_mac_miim_write(PHY_CTRL, t | MII_CR_RESTART_AUTO_NEG);
}

static bool 
unprogrammed(const u2_mac_addr_t *t)
{
  int i;
  bool all_zeros = true;
  bool all_ones =  true;
  for (i = 0; i < 6; i++){
    all_zeros &= t->addr[i] == 0x00;
    all_ones  &= t->addr[i] == 0xff;
  }
  return all_ones | all_zeros;
}

static int8_t src_addr_initialized = false;
static u2_mac_addr_t src_addr = {{
    0x00, 0x50, 0xC2, 0x85, 0x3f, 0xff
  }};

const u2_mac_addr_t *
ethernet_mac_addr(void)
{
  if (!src_addr_initialized){    // fetch from eeprom
    src_addr_initialized = true;

    // if we're simulating, don't read the EEPROM model, it's REALLY slow
    if (hwconfig_simulation_p())	
      return &src_addr;
    
    u2_mac_addr_t tmp;
    bool ok = eeprom_read(I2C_ADDR_MBOARD, MBOARD_MAC_ADDR, &tmp.addr[0], 6);
    if (!ok || unprogrammed(&tmp)){
      // use the default
    }
    else
      src_addr = tmp;
  }

  return &src_addr;
}

bool
ethernet_set_mac_addr(const u2_mac_addr_t *t)
{
  bool ok = eeprom_write(I2C_ADDR_MBOARD, MBOARD_MAC_ADDR, &t->addr[0], 6);
  if (ok){
    src_addr = *t;
    src_addr_initialized = true;
    eth_mac_set_addr(t);
  }

  return ok;
}

int
ethernet_check_errors(void)
{
  // these registers are reset when read
  
  int	r = 0;
  /*
  if (eth_mac_read_rmon(0x05) != 0)
    r |= RME_RX_CRC;
  if (eth_mac_read_rmon(0x06) != 0)
    r |= RME_RX_FIFO_FULL;
  if (eth_mac_read_rmon(0x07) != 0)
    r |= RME_RX_2SHORT_2LONG;
  
  if (eth_mac_read_rmon(0x25) != 0)
    r |= RME_TX_JAM_DROP;
  if (eth_mac_read_rmon(0x26) != 0)
    r |= RME_TX_FIFO_UNDER;
  if (eth_mac_read_rmon(0x27) != 0)
    r |= RME_TX_FIFO_OVER;
  */
  return r;
}
