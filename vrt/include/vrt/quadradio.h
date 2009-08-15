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
#ifndef INCLUDED_VRT_QUADRADIO_H
#define INCLUDED_VRT_QUADRADIO_H

#include <vrt/rx.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef enum{
    VRT_TEST_SIG_NORMAL=0,
    VRT_TEST_SIG_ZEROS=1,
    VRT_TEST_SIG_ONES=2,
    VRT_TEST_SIG_TOGGLE=3,
    VRT_TEST_SIG_RAMP=4,
    VRT_TEST_SIG_CUSTOM=5,  
      
    } vrt_test_sig_t;

namespace vrt {

  /*
   * We're not committing to this interface.  It's just here so we can make progress...
   *
   * This implements the ad-hoc control for bringup and has-a vrt::rx
   */
  class quadradio
  {
    int		   d_ctrl_fd;	       // socket for control
    struct in_addr d_ctrl_port_inaddr; // our ip addr
    int		   d_data_fd;	       // socket for data (owned by d_rx)
    int		   d_data_port;	       // our data port number
    vrt::rx::sptr  d_rx;	       // has-a rx
    
    int		   d_band_select;	       // band select setting
    int		   d_rx_antenna;	       // antenna type rf/cal
    int		   d_attenuation0;	       // attenuation setting
    int		   d_attenuation1;	       // attenuation setting
    bool	   d_10dB_atten;	       // 10dB attenuation on/of

    static bool
    open_sockets(const char *quad_radio_ip, int quad_radio_ctrl_port,
		 int *ctrl_fd_ptr, struct in_addr *ctrl_port_inaddr,
		 int *data_fd_ptr, int *data_port_ptr);

    static bool
    send_rx_command(int ctrl_fd, bool start,
		    struct in_addr addr, int data_port, int samples_per_pkt, int siggen_param);

    static bool
    send_stop_rx_command(int ctrl_fd);
    
    static int control_port() { return 790; }
    int data_socket_fd() const { return d_data_fd; }

    bool open(const char *ip);
    
    void update_dboard_pins(void);

  public:
    typedef boost::shared_ptr<quadradio> sptr;

    quadradio(const std::string &ip, size_t rx_bufsize = 0);
    ~quadradio();

    vrt::rx::sptr vrt_rx() const { return d_rx; }

    bool start_streaming(int samples_per_pkt = 0);
    bool stop_streaming();


    /* convenience methods that ultimately write the dboard pins */
    bool set_center_freq(double target_freq);
    bool set_band_select(const std::string &band);
    //void set_10dB_atten(bool on);
    bool set_attenuation0(int attenuation);
    bool select_rx_antenna(const std::string &ant);
    bool set_attenuation1(int attenuation);
    
    /* convenience methods that ultimately call set_hsadc_conf */
    void set_adc_gain(bool on);
    void set_dc_offset_comp(bool on);
    void set_digital_gain(float gain);
    void set_test_signal(vrt_test_sig_t type);
    
    /* primitives */
    bool set_setting_reg(int regno, int value);
    bool set_mem32(int addr, int value);	// poke a 32-bit value
    bool set_lo_freq(double freq);
    bool set_cal_freq(double freq);
    bool set_beamforming(int32_t gains[8]);
    /*
     * The first parameter for these is a bitmask which indicates which
     * daughterboard or daughterboards to apply the operation to.
     * 0x1	-> dboard 0
     * 0x2	-> dboard 1
     * 0x3	-> dboard 0 and 1...
     */
    bool set_dboard_pins(int dboard_bitmask, int v);
    bool set_hsadc_conf(int dboard_bitmask, int regno, int value);
    bool set_lsdac(int dboard_bitmask, int which_dac, int value);

  };

};


#endif /* INCLUDED_QUADRADIO_H */
