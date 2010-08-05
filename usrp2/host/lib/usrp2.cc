/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include "usrp2_impl.h"
#include <vector>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include <string>
#include <stdexcept>
#include <cstdio>

namespace usrp2 {

  // --- Table of weak pointers to usrps we know about ---

  // (Could be cleaned up and turned into a template)

  struct usrp_table_entry {
    // inteface + normalized mac addr ("eth0:01:23:45:67:89:ab")
    std::string	key;
    boost::weak_ptr<usrp2>  value;

    usrp_table_entry(const std::string &_key, boost::weak_ptr<usrp2> _value)
      : key(_key), value(_value) {}
  };

  typedef std::vector<usrp_table_entry> usrp_table;

  static boost::mutex s_table_mutex;
  static usrp_table s_table;

  usrp2::sptr
  usrp2::find_existing_or_make_new(const std::string &ifc, props *pr, size_t rx_bufsize)
  {
    std::string key = ifc + ":" + pr->addr;

    boost::mutex::scoped_lock	guard(s_table_mutex);

    for (usrp_table::iterator p = s_table.begin(); p != s_table.end();){
      if (p->value.expired())	// weak pointer is now dead
	p = s_table.erase(p);	// erase it
      else {
	if (key == p->key)	// found it
	  return usrp2::sptr(p->value);
	else
	  ++p;			// keep looking
      }
    }

    // We don't have the USRP2 we're looking for

    // create a new one and stick it in the table.
    usrp2::sptr r(new usrp2(ifc, pr, rx_bufsize));
    usrp_table_entry t(key, r);
    s_table.push_back(t);

    return r;
  }

  // --- end of table code ---

  static bool
  parse_mac_addr(const std::string &s, std::string &ns)
  {
    u2_mac_addr_t p;

    p.addr[0] = 0x00;		// Matt's IAB
    p.addr[1] = 0x50;
    p.addr[2] = 0xC2;
    p.addr[3] = 0x85;
    p.addr[4] = 0x30;
    p.addr[5] = 0x00;

    int len = s.size();
    switch (len) {

    case 5:
      if (sscanf(s.c_str(), "%hhx:%hhx", &p.addr[4], &p.addr[5]) != 2)
	return false;
      break;

    case 17:
      if (sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		 &p.addr[0], &p.addr[1], &p.addr[2],
		 &p.addr[3], &p.addr[4], &p.addr[5]) != 6)
	return false;
      break;

    default:
      return false;
    }

    char buf[128];
    snprintf(buf, sizeof(buf),
	     "%02x:%02x:%02x:%02x:%02x:%02x",
	     p.addr[0],p.addr[1],p.addr[2],
	     p.addr[3],p.addr[4],p.addr[5]);
    ns = std::string(buf);
    return true;
  }

  usrp2::sptr
  usrp2::make(const std::string &ifc, const std::string &addr, size_t rx_bufsize)
  {
    std::string naddr = "";
    if (addr != "" && !parse_mac_addr(addr, naddr))
      throw std::runtime_error("Invalid MAC address");

    props_vector_t u2s = find(ifc, naddr);
    int n = u2s.size();

    if (n == 0) {
      if (addr == "")
	throw std::runtime_error("No USRPs found on interface " + ifc);
      else
	throw std::runtime_error("No USRP found with addr " + addr + " on interface " + ifc);
    }

    if (n > 1)
      throw std::runtime_error("Multiple USRPs found on interface; must select by MAC address.");

    return find_existing_or_make_new(ifc, &u2s[0], rx_bufsize);
  }

  // Private constructor.  Sole function is to create an impl.
  usrp2::usrp2(const std::string &ifc, props *p, size_t rx_bufsize)
    : d_impl(new usrp2::impl(ifc, p, rx_bufsize))
  {
    // NOP
  }

  // Public class destructor.  d_impl will auto-delete.
  usrp2::~usrp2()
  {
    // NOP
  }

  std::string
  usrp2::mac_addr()
  {
    return d_impl->mac_addr();
  }

  std::string
  usrp2::interface_name()
  {
    return d_impl->interface_name();
  }

  // Receive

  bool
  usrp2::set_rx_antenna(int ant){
    return d_impl->set_rx_antenna(ant);
  }

  bool
  usrp2::set_rx_gain(double gain)
  {
    return d_impl->set_rx_gain(gain);
  }

  double
  usrp2::rx_gain_min()
  {
    return d_impl->rx_gain_min();
  }

  double
  usrp2::rx_gain_max()
  {
    return d_impl->rx_gain_max();
  }

  double
  usrp2::rx_gain_db_per_step()
  {
    return d_impl->rx_gain_db_per_step();
  }

  bool
  usrp2::set_rx_lo_offset(double frequency)
  {
    return d_impl->set_rx_lo_offset(frequency);
  }

  bool
  usrp2::set_rx_center_freq(double frequency, tune_result *result)
  {
    return d_impl->set_rx_center_freq(frequency, result);
  }

  double
  usrp2::rx_freq_min()
  {
    return d_impl->rx_freq_min();
  }

  double
  usrp2::rx_freq_max()
  {
    return d_impl->rx_freq_max();
  }

  bool
  usrp2::set_rx_decim(int decimation_factor)
  {
    return d_impl->set_rx_decim(decimation_factor);
  }

  int
  usrp2::rx_decim()
  {
    return d_impl->rx_decim();
  }

  bool
  usrp2::set_rx_scale_iq(int scale_i, int scale_q)
  {
    return d_impl->set_rx_scale_iq(scale_i, scale_q);
  }

  bool
  usrp2::start_rx_streaming(unsigned int channel, unsigned int items_per_frame)
  {
    return d_impl->start_rx_streaming(channel, items_per_frame);
  }

  bool
  usrp2::start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time)
  {
    return d_impl->start_rx_streaming_at(channel, items_per_frame,time);
  }

  bool
  usrp2::sync_and_start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time)
  {
    return d_impl->sync_and_start_rx_streaming_at(channel, items_per_frame, time);
  }

  bool
  usrp2::rx_samples(unsigned int channel, rx_sample_handler *handler)
  {
    return d_impl->rx_samples(channel, handler);
  }

  bool
  usrp2::stop_rx_streaming(unsigned int channel)
  {
    return d_impl->stop_rx_streaming(channel);
  }

  unsigned int
  usrp2::rx_overruns()
  {
    return d_impl->rx_overruns();
  }

  unsigned int
  usrp2::rx_missing()
  {
    return d_impl->rx_missing();
  }

  // Transmit

  bool
  usrp2::set_tx_antenna(int ant){
    return d_impl->set_tx_antenna(ant);
  }

  bool
  usrp2::set_tx_gain(double gain)
  {
    return d_impl->set_tx_gain(gain);
  }

  double
  usrp2::tx_gain_min()
  {
    return d_impl->tx_gain_min();
  }

  double
  usrp2::tx_gain_max()
  {
    return d_impl->tx_gain_max();
  }

  double
  usrp2::tx_gain_db_per_step()
  {
    return d_impl->tx_gain_db_per_step();
  }

  bool
  usrp2::set_tx_lo_offset(double frequency)
  {
    return d_impl->set_tx_lo_offset(frequency);
  }

  bool
  usrp2::set_tx_center_freq(double frequency, tune_result *result)
  {
    return d_impl->set_tx_center_freq(frequency, result);
  }

  double
  usrp2::tx_freq_min()
  {
    return d_impl->tx_freq_min();
  }

  double
  usrp2::tx_freq_max()
  {
    return d_impl->tx_freq_max();
  }


  bool
  usrp2::set_tx_interp(int interpolation_factor)
  {
    return d_impl->set_tx_interp(interpolation_factor);
  }

  int
  usrp2::tx_interp()
  {
    return d_impl->tx_interp();
  }

  void
  usrp2::default_tx_scale_iq(int interpolation_factor, int *scale_i, int *scale_q)
  {
    d_impl->default_tx_scale_iq(interpolation_factor, scale_i, scale_q);
  }

  bool
  usrp2::set_tx_scale_iq(int scale_i, int scale_q)
  {
    return d_impl->set_tx_scale_iq(scale_i, scale_q);
  }

  bool
  usrp2::tx_32fc(unsigned int channel,
		 const std::complex<float> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata)
  {
    return d_impl->tx_32fc(channel, samples, nsamples, metadata);
  }

  bool
  usrp2::tx_16sc(unsigned int channel,
		 const std::complex<int16_t> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata)
  {
    return d_impl->tx_16sc(channel, samples, nsamples, metadata);
  }

  bool
  usrp2::tx_raw(unsigned int channel,
		const uint32_t *items,
		size_t nitems,
		const tx_metadata *metadata)
  {
    return d_impl->tx_raw(channel, items, nitems, metadata);
  }

  // miscellaneous methods

  bool
  usrp2::config_mimo(int flags)
  {
    return d_impl->config_mimo(flags);
  }

  bool
  usrp2::fpga_master_clock_freq(long *freq)
  {
    return d_impl->fpga_master_clock_freq(freq);
  }

  bool
  usrp2::adc_rate(long *rate)
  {
    return d_impl->adc_rate(rate);
  }

  bool
  usrp2::dac_rate(long *rate)
  {
    return d_impl->dac_rate(rate);
  }

  bool
  usrp2::tx_daughterboard_id(int *dbid)
  {
    return d_impl->tx_daughterboard_id(dbid);
  }

  bool
  usrp2::rx_daughterboard_id(int *dbid)
  {
    return d_impl->rx_daughterboard_id(dbid);
  }


  // low level methods

  bool
  usrp2::burn_mac_addr(const std::string &new_addr)
  {
    return d_impl->burn_mac_addr(new_addr);
  }

  bool
  usrp2::sync_to_pps()
  {
    return d_impl->sync_to_pps();
  }

  bool
  usrp2::sync_every_pps(bool enable)
  {
    return d_impl->sync_every_pps(enable);
  }

  std::vector<uint32_t>
  usrp2::peek32(uint32_t addr, uint32_t words)
  {
    return d_impl->peek32(addr, words);
  }

  bool
  usrp2::poke32(uint32_t addr, const std::vector<uint32_t> &data)
  {
    return d_impl->poke32(addr, data);
  }

  bool
  usrp2::set_gpio_ddr(int bank, uint16_t value, uint16_t mask)
  {
    return d_impl->set_gpio_ddr(bank, value, mask);
  }

  bool
  usrp2::set_gpio_sels(int bank, std::string src)
  {
    return d_impl->set_gpio_sels(bank, src);
  }

  bool
  usrp2::write_gpio(int bank, uint16_t value, uint16_t mask)
  {
    return d_impl->write_gpio(bank, value, mask);
  }

  bool
  usrp2::read_gpio(int bank, uint16_t *value)
  {
    return d_impl->read_gpio(bank, value);
  }

  bool
  usrp2::enable_gpio_streaming(int bank, int enable)
  {
    return d_impl->enable_gpio_streaming(bank, enable);
  }

} // namespace usrp2

std::ostream& operator<<(std::ostream &os, const usrp2::props &x)
{
  os << x.addr;

  char buf[128];
  snprintf(buf, sizeof(buf)," hw_rev = 0x%04x", x.hw_rev);

  os << buf;
  return os;
}
