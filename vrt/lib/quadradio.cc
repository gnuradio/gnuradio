/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vrt/quadradio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <math.h>

#define	MIN_IP_LOCAL_PORT	32768
#define	MAX_IP_LOCAL_PORT	61000

#define ALL_DBOARDS	0xf

static bool
send_and_check(int fd, void *buf, size_t len)
{
  int r = send(fd, buf, len, 0);
  if (r < 0){
    perror("send");
    return false;
  }
  if ((size_t) r != len){
    fprintf(stderr, "send: short return value.  expected %zd, got %d\n", len, r);
    return false;
  }
  return true;
}


vrt::quadradio::quadradio(const std::string &ip, size_t rx_bufsize)
  : d_ctrl_fd(0), d_data_fd(0), d_data_port(0),
  d_band_select(VRT_BAND_SEL_A), d_rx_antenna(0), d_attenuation0(0), d_attenuation1(0)//d_10dB_atten(true)
{
  if (!open(ip.c_str()))
    throw std::runtime_error("vrt::quadradio: failed to open " + ip + "\n");

  d_rx = vrt::rx::make(data_socket_fd(), rx_bufsize);
  set_test_signal(VRT_TEST_SIG_NORMAL);
}

vrt::quadradio::~quadradio()
{
  ::close(d_ctrl_fd);
}

bool
vrt::quadradio::open(const char *ip)
{
  return open_sockets(ip, control_port(),
		      &d_ctrl_fd, &d_ctrl_port_inaddr,
		      &d_data_fd, &d_data_port);
}

bool
vrt::quadradio::start_streaming(int rxdspno, int samples_per_pkt)
{
  return send_rx_command(d_ctrl_fd, rxdspno, true, d_ctrl_port_inaddr,
			 d_data_port, samples_per_pkt);
}

bool
vrt::quadradio::stop_streaming(int rxdspno)
{
  return send_stop_rx_command(d_ctrl_fd, rxdspno);
}

bool
vrt::quadradio::set_center_freq(double target_freq){
    if (target_freq < 700e6) return false;
    if (target_freq <= 1.0e9) return set_band_select(VRT_BAND_SEL_A);
    if (target_freq <= 1.5e9) return set_band_select(VRT_BAND_SEL_B);
    if (target_freq <= 2.2e9) return set_band_select(VRT_BAND_SEL_C);
    if (target_freq <= 3.0e9) return set_band_select(VRT_BAND_SEL_D);
    return false;
}

bool
vrt::quadradio::set_band_select(vrt_band_sel_t band){
    d_band_select = band;
    update_dboard_pins();
    return true;
}

//void
//vrt::quadradio::set_10dB_atten(bool on){
//    d_10dB_atten = on;
//    update_dboard_pins();
//}

bool
vrt::quadradio::select_rx_antenna(const std::string &ant){
    if (ant == "rf") d_rx_antenna = 0;
    else if (ant == "cal") d_rx_antenna = 1;
    else return true;
    update_dboard_pins();
    return true;
}

bool
vrt::quadradio::set_attenuation0(int attenuation){
    if (attenuation < 0 || attenuation > 31) return false;
    d_attenuation0 = attenuation;
    update_dboard_pins();
    return true;
}

bool
vrt::quadradio::set_attenuation1(int attenuation){
    if (attenuation < 0 || attenuation > 31) return false;
    d_attenuation1 = attenuation;
    update_dboard_pins();
    return true;
}

//bit reversal, length in bits
static int reverse_bits(int input, int len){
    int reversed = 0;
    for (int i = 0; i < len; i++){
        reversed += (input & (1<<i))?(1 << (len-i-1)):0;
    }
    return reversed;
}

void
vrt::quadradio::update_dboard_pins(void){
    //convert the band ID to bits
    int band_select;
    switch (d_band_select){
        case VRT_BAND_SEL_A: band_select = 3; break;
        case VRT_BAND_SEL_B: band_select = 2; break;
        case VRT_BAND_SEL_C: band_select = 1; break;
        case VRT_BAND_SEL_D: band_select = 0; break;
        default: band_select = 0;
    }
    //calculate the control bits
    int db_ctrl = \
        ((reverse_bits(d_attenuation0, 5)  & 0x1f) << 10) | \
        ((reverse_bits(~d_attenuation1, 5) & 0x1f) << 03) | \
        ((band_select                      & 0x03) << 01) | \
        ((d_rx_antenna                     & 0x01) << 00);
    set_dboard_pins(ALL_DBOARDS, db_ctrl);  // FIXME sets them all
}

void
vrt::quadradio::set_adc_gain(bool on){
  set_hsadc_conf(ALL_DBOARDS, 0x14, on ? 0x90 : 0x80);
}

void
vrt::quadradio::set_dc_offset_comp(bool on){
    if (on) {
        set_hsadc_conf(ALL_DBOARDS, 0x1B, 0x80);
        set_hsadc_conf(ALL_DBOARDS, 0x1A, 0x00); //bits 6:4 set time constant
    }
    else set_hsadc_conf(ALL_DBOARDS, 0x1B, 0x00);
}

void
vrt::quadradio::set_digital_gain(float gain){
    int gain_q1 = static_cast<int>(round(gain*2.0));
    set_hsadc_conf(ALL_DBOARDS, 0x17, gain_q1);
}

void
vrt::quadradio::set_test_signal(vrt_test_sig_t type){
    set_hsadc_conf(ALL_DBOARDS, 0x16, type);
}

bool
vrt::quadradio::open_sockets(const char *quad_radio_ip, int quad_radio_ctrl_port,
			     int *ctrl_fd_ptr, struct in_addr *ctrl_port_inaddr,
			     int *data_fd_ptr, int *data_port_ptr)
{
  int	ctrl_fd;	// socket for control
  int	data_fd;	// socket fd for data
  int	data_port;	// our port number

  //
  // create a udp socket and connect it to the quad radio control port
  //

  ctrl_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (ctrl_fd == -1){
    perror("socket: ctrl_fd");
    return false;
  }

  struct sockaddr_in si_other;
  memset(&si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(quad_radio_ctrl_port);
  if (inet_pton(AF_INET, quad_radio_ip, &si_other.sin_addr) <= 0){
    perror("inet_pton");
    return false;
  }

  if (connect(ctrl_fd, (struct sockaddr *) &si_other, sizeof(si_other)) != 0){
    perror("connect");
    return false;
  }

  // get our ip address associated with the interface connected to the control port

  struct sockaddr_in si_me;
  memset(&si_me, 0, sizeof(si_me));
  socklen_t sockname_len = sizeof(si_me);
  if (getsockname(ctrl_fd, (struct sockaddr *) &si_me, &sockname_len) != 0){
    perror("getsockname");
  }
  
  *ctrl_port_inaddr = si_me.sin_addr;

  if (1){
    char buf[128];
    const char *s = inet_ntop(si_me.sin_family, &si_me.sin_addr, buf, sizeof(buf));
    if (s == 0){
      perror("inet_ntop");
      return false;
    }
    // printf("our ip addr associated with ctrl port: %s\n", s);
  }
  
  //
  // create a udp socket to use to receive data
  //

  data_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (data_fd == -1){
    perror("socket: data_fd");
    return false;
  }

  // bind it to a local port on the interface that connects to the ctrl port.
  // FIXME this assumes that interface connected to the control port and the
  //   interface connected to the data port are the same.  If we're using
  //   both ethernet ports on the quad radio, this may not be the case.

  data_port = -1;
  for (int port = MIN_IP_LOCAL_PORT; port <= MAX_IP_LOCAL_PORT; port++){
    struct sockaddr_in si_me;
    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(data_fd, (struct sockaddr *) &si_me, sizeof(si_me)) == 0){	// found one!
      data_port = port;
      break;
    }
  }

  if (data_port == -1){
    fprintf(stderr, "failed to bind to a local port\n");
    return false;
  }

  // printf("our data port = %d\n", data_port);

  *ctrl_fd_ptr = ctrl_fd;
  *data_fd_ptr = data_fd;
  *data_port_ptr = data_port;

  return true;
}

// ------------------------------------------------------------------------

bool
vrt::quadradio::send_rx_command(int ctrl_fd, int rxdspno, bool start,
				struct in_addr addr, int data_port,
				int samples_per_pkt)
{
  uint32_t cmd[7];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(0);		   // id: rx_streaming
  cmd[2] = htonl(start ? 1: 0);	   // start or stop?
  cmd[3] = addr.s_addr;	  	   // ip address to send data to (already network endian)
  cmd[4] = htonl(data_port);	   // port to send data to
  cmd[5] = htonl(samples_per_pkt);
  cmd[6] = htonl(rxdspno);	   // the DSP pipeline to configure

  return send_and_check(ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::send_stop_rx_command(int ctrl_fd, int rxdspno)
{
  struct in_addr in_addr;
  in_addr.s_addr = 0;
  return send_rx_command(ctrl_fd, rxdspno, false, in_addr, 0, 0);
}

bool
vrt::quadradio::set_dboard_pins(int dboard_bitmask, int v)
{
  uint32_t cmd[4];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(1);		   // id: dboard_pins
  cmd[2] = htonl(dboard_bitmask);
  cmd[3] = htonl(v);	   	   // value

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}
  
bool
vrt::quadradio::set_setting_reg(int regno, int value)
{
  uint32_t cmd[4];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(2);		   // id: SR
  cmd[2] = htonl(regno);
  cmd[3] = htonl(value);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_hsadc_conf(int dboard_bitmask, int regno, int value)
{
  uint32_t cmd[5];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(3);		   // id: HSADC_CONF
  cmd[2] = htonl(dboard_bitmask);
  cmd[3] = htonl(regno);
  cmd[4] = htonl(value);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_lsdac(int dboard_bitmask, int which_dac, int value)
{
  uint32_t cmd[5];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(4);		   // id: LSDAC
  cmd[2] = htonl(dboard_bitmask);
  cmd[3] = htonl(which_dac);
  cmd[4] = htonl(value);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_mem32(int addr, int value)
{
  uint32_t cmd[4];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(5);		   // id: MEM32
  cmd[2] = htonl(addr);
  cmd[3] = htonl(value);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_lo_freq(double freq)
{
  uint64_t lo_freq = uint64_t(freq * (uint64_t(1)<<20)); //q20 format
  uint32_t cmd[4];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(6);		   // id: lo freq
  cmd[2] = htonl((lo_freq >> 32) & 0xffffffff);
  cmd[3] = htonl((lo_freq >> 0) & 0xffffffff);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_cal_freq(double freq)
{
  uint64_t cal_freq = uint64_t(freq * (uint64_t(1)<<20)); //q20 format
  uint32_t cmd[4];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(7);		   // id: cal freq
  cmd[2] = htonl((cal_freq >> 32) & 0xffffffff);
  cmd[3] = htonl((cal_freq >> 0) & 0xffffffff);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_beamforming(int32_t gains[8]){
  uint32_t cmd[2+8];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(8);		   // id: beamformin
  for (int i = 0; i < 8; i++){
    //printf("%d\n", gains[i]);
    cmd[i+2] = htonl(gains[i]); 
  }
  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}

bool
vrt::quadradio::set_cal_enb(bool enb)
{
  uint32_t cmd[3];
  cmd[0] = htonl(0);		   // verb: set
  cmd[1] = htonl(9);		   // id: cal enb
  cmd[2] = htonl(enb);

  return send_and_check(d_ctrl_fd, cmd, sizeof(cmd));
}
