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
#ifndef INCLUDED_VRT_QUADRADIO_SOURCE_32FC_H
#define INCLUDED_VRT_QUADRADIO_SOURCE_32FC_H

#include <vrt_source_32fc.h>

namespace vrt {
  class quadradio;
};

class vrt_quadradio_source_32fc;
typedef boost::shared_ptr<vrt_quadradio_source_32fc> vrt_quadradio_source_32fc_sptr;

vrt_quadradio_source_32fc_sptr
vrt_make_quadradio_source_32fc(const std::string &ip,
			       size_t rx_bufsize = 0,
			       size_t samples_per_pkt = 0,
			       int rxdspno = 0);

class vrt_quadradio_source_32fc : public vrt_source_32fc
{
  size_t				d_samples_per_pkt;
  boost::shared_ptr<vrt::quadradio>	d_qr;
  int 					d_rxdspno;

  vrt_quadradio_source_32fc(const std::string &ip, size_t rx_bufsize,
			    size_t samples_per_pkt, int rxdspno);

  friend vrt_quadradio_source_32fc_sptr
  vrt_make_quadradio_source_32fc(const std::string &ip, size_t rx_bufsize,
				 size_t samples_per_pkt, int rxdspno);

public:
  virtual ~vrt_quadradio_source_32fc();
  virtual vrt::rx::sptr vrt_rx() const;

  /*!
   * \brief Called by scheduler when starting flowgraph
   */
  virtual bool start();
  
  /*!
   * \brief Called by scheduler when stopping flowgraph
   */
  virtual bool stop();

  /*!
   * \brief Set the LO frequency (actually just sets the band select for now).
   */
  bool set_center_freq(double target_freq);

  /*!
   * \brief Set the band select dboard bits.
   */
  bool set_band_select(int band);
  int get_band_select(void);

  /*!
   * \brief Turn the 10 dB attenuation on/off.
   */
  //void set_10dB_atten(bool on);

  /*!
   * \brief Set the antenna type to the main rf or calibrator.
   * \param ant "rf" or "cal"
   */
  bool select_rx_antenna(const std::string &ant);

  /*!
   * \brief Set the attenuation.
   * \param attenuation 0 to 31 in dB
   */
  bool set_attenuation0(int attenuation);
  bool set_attenuation1(int attenuation);

  void set_iq_imbal_taps(const std::vector<gr_complex> taps);

  void set_adc_gain(bool on);
  void set_dc_offset_comp(bool on);
  void set_digital_gain(float gain);
  void set_test_signal(int type);

  bool set_setting_reg(int regno, int value);

  /*!
   * \brief write \p v to daugherboard control setting register
   */
  bool set_dboard_pins(int which_dboard, int v);
  bool set_hsadc_conf(int which_dboard, int regno, int value);
  bool set_lsdac(int which_dboard, int which_dac, int value);
  bool set_mem32(int addr, int value);
  bool set_lo_freq(double freq);
  bool set_cal_freq(double freq);
  bool set_beamforming(std::vector<gr_complex> gains);
  bool set_cal_enb(bool enb);
  
  //caldiv public access methods
  double get_cal_freq(void){return d_cal_freq;}
  bool get_cal_locked(void){return d_cal_locked;}
  bool get_cal_enabled(void){return d_cal_enabled;}
  double get_lo_freq(void){return d_lo_freq;}
  bool get_lo_locked(void){return d_lo_locked;}
  double get_caldiv_temp(void){return d_caldiv_temp;}
  int get_caldiv_serial(void){return d_caldiv_rev;}
  int get_caldiv_revision(void){return d_caldiv_ser;}
  
  //gps public access methods
  int get_utc_time(void){return d_utc_time;}
  double get_altitude(void){return d_altitude;}
  double get_longitude(void){return d_longitude;}
  double get_latitude(void){return d_latitude;}
};



#endif /* INCLUDED_VRT_QUADRADIO_SOURCE_32FC_H */
