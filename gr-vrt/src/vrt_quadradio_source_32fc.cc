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
#include <vrt_quadradio_source_32fc.h>
#include <vrt/quadradio.h>
#include <vrt/rx_packet_handler.h>
#include <cstdio>

vrt_quadradio_source_32fc_sptr
vrt_make_quadradio_source_32fc(const std::string &ip, 
			       size_t rx_bufsize,
			       size_t samples_per_pkt)
{
  return gnuradio::get_initial_sptr(new vrt_quadradio_source_32fc(ip,
								  rx_bufsize,
								  samples_per_pkt));
}

vrt_quadradio_source_32fc::vrt_quadradio_source_32fc(const std::string &ip,
						     size_t rx_bufsize,
						     size_t samples_per_pkt)
  : vrt_source_32fc("quadradio_source_32fc"),
    d_samples_per_pkt(samples_per_pkt == 0 ? 800 : samples_per_pkt),
    d_qr(vrt::quadradio::sptr(new vrt::quadradio(ip, rx_bufsize)))
{
}

vrt_quadradio_source_32fc::~vrt_quadradio_source_32fc()
{
  d_qr->stop_streaming();
}

vrt::rx::sptr 
vrt_quadradio_source_32fc::vrt_rx() const
{
  return d_qr->vrt_rx();
}

bool 
vrt_quadradio_source_32fc::start()
{
  // throw away any stale packets before starting
  vrt::rx_packet_handler nop;
  vrt_rx()->rx_packets(&nop, true);
  d_checker.resync();

  return d_qr->start_streaming(d_samples_per_pkt);
}  

bool
vrt_quadradio_source_32fc::stop()
{
  return d_qr->stop_streaming();
}

bool 
vrt_quadradio_source_32fc::set_dboard_pins(int which_dboard, int v)
{
  return d_qr->set_dboard_pins(which_dboard, v);
}

bool 
vrt_quadradio_source_32fc::set_center_freq(double target_freq)
{
  return d_qr->set_center_freq(target_freq);
}

bool 
vrt_quadradio_source_32fc::set_band_select(const std::string &band)
{
  return d_qr->set_band_select(band);
}

std::string 
vrt_quadradio_source_32fc::get_band_select(void)
{
  return d_qr->get_band_select();
}

//void 
//vrt_quadradio_source_32fc::set_10dB_atten(bool on)
//{
//  return d_qr->set_10dB_atten(on);
//}

bool 
vrt_quadradio_source_32fc::select_rx_antenna(const std::string &ant)
{
  return d_qr->select_rx_antenna(ant);
}

bool 
vrt_quadradio_source_32fc::set_attenuation0(int attenuation)
{
  return d_qr->set_attenuation0(attenuation);
}

bool 
vrt_quadradio_source_32fc::set_attenuation1(int attenuation)
{
  return d_qr->set_attenuation1(attenuation);
}

void
vrt_quadradio_source_32fc::set_adc_gain(bool on){
  d_qr->set_adc_gain(on);
}

void
vrt_quadradio_source_32fc::set_dc_offset_comp(bool on){
  d_qr->set_dc_offset_comp(on);
}

void
vrt_quadradio_source_32fc::set_digital_gain(float gain){
  d_qr->set_digital_gain(gain);
}

void
vrt_quadradio_source_32fc::set_test_signal(int type){
  d_qr->set_test_signal(static_cast<vrt_test_sig_t>(type));
}

bool
vrt_quadradio_source_32fc::set_setting_reg(int regno, int value)
{
  return d_qr->set_setting_reg(regno, value);
}

bool
vrt_quadradio_source_32fc::set_hsadc_conf(int which_dboard, int regno, int value)
{
  return d_qr->set_hsadc_conf(which_dboard, regno, value);
}

bool
vrt_quadradio_source_32fc::set_lsdac(int which_dboard, int which_dac, int value)
{
  return d_qr->set_lsdac(which_dboard, which_dac, value);
}

bool
vrt_quadradio_source_32fc::set_mem32(int addr, int value)
{
  return d_qr->set_mem32(addr, value);
}

bool
vrt_quadradio_source_32fc::set_lo_freq(double freq)
{
  return d_qr->set_lo_freq(freq);
}

bool
vrt_quadradio_source_32fc::set_cal_freq(double freq)
{
  return d_qr->set_cal_freq(freq);
}

/*--------------------------------------------------------------------*/
#define IQ_IMBAL_NUM_TAPS 30
#define IQ_IMBAL_REG_NO 162

//helper function to set the iq imbalance register with a tap
static int get_iq_imbal_reg(bool real, bool init, float tap){
  int val = int(round(tap));
  val &= 0x1ffffff; //lower 25 bits for tap
  val |= (real?0:1) << 30; //30th bit for filter type
  val |= (init?1:0) << 31; //31st bit for initialization
  printf("Reg %d Val %x\n", IQ_IMBAL_REG_NO, val);
  return val;
}

void
vrt_quadradio_source_32fc::set_iq_imbal_taps(const std::vector<gr_complex> taps){
  int i = 0;
  /* set the real part of the taps */
  get_iq_imbal_reg(true, true, 0);
  for (i = 0; i < IQ_IMBAL_NUM_TAPS; i++){
      set_setting_reg(IQ_IMBAL_REG_NO, get_iq_imbal_reg(true, false, taps[IQ_IMBAL_NUM_TAPS-i-1].real()));
  }
  get_iq_imbal_reg(false, true, 0);
  /* set the imaginary part of the taps */
  for (i = 0; i < IQ_IMBAL_NUM_TAPS; i++){
      set_setting_reg(IQ_IMBAL_REG_NO, get_iq_imbal_reg(false, false, taps[IQ_IMBAL_NUM_TAPS-i-1].imag()));
  }
}

bool
vrt_quadradio_source_32fc::set_beamforming(std::vector<gr_complex> gains){
  int32_t gains_ints[8];
  for (int i = 0; i < 4; i++){
    gains_ints[2*i] = int32_t(gains[i].real());
    gains_ints[2*i+1] = int32_t(gains[i].imag());
  }
 return d_qr->set_beamforming(gains_ints);
}

