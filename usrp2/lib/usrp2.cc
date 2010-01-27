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
#include "eth_ctrl_transport.h"
#include "udp_ctrl_transport.h"
#include "eth_data_transport.h"

namespace usrp2 {

  // --- Table of weak pointers to usrps we know about ---

  // (Could be cleaned up and turned into a template)

  struct usrp_table_entry {
    // inteface + normalized mac addr ("eth0:01:23:45:67:89:ab")
    std::string	key;
    boost::weak_ptr<usrp2::usrp2>  value;

    usrp_table_entry(const std::string &_key, boost::weak_ptr<usrp2::usrp2> _value)
      : key(_key), value(_value) {}
  };

  typedef std::vector<usrp_table_entry> usrp_table;

  static boost::mutex s_table_mutex;
  static usrp_table s_table;

  usrp2::sptr
  usrp2::find_existing_or_make_new(const props &pr, size_t rx_bufsize)
  {
    std::string key = props::to_string(pr);

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
    usrp2::sptr r(new usrp2::usrp2(pr, rx_bufsize));
    usrp_table_entry t(key, r);
    s_table.push_back(t);

    return r;
  }

  // --- end of table code ---

  usrp2::sptr
  usrp2::make(const props &hint, size_t rx_bufsize)
  {

    props_vector_t props = find(hint);
    int n = props.size();

    if (n == 0)
      throw std::runtime_error("No USRPs found");

    if (n > 1)
      throw std::runtime_error("Multiple USRPs found; must select by args.");

    return find_existing_or_make_new(props[0], rx_bufsize);
  }

  // Private constructor.  Sole function is to create an impl.
  usrp2::usrp2(const props &p, size_t rx_bufsize): d_props(p)
  {
    transport::sptr ctrl_transport;
    transport::sptr data_transport;

    //create the transports based on the usrp type
    switch (p.type){
    case USRP_TYPE_AUTO: throw std::runtime_error("Not supported");
    case USRP_TYPE_ETH:
        ctrl_transport = transport::sptr(new eth_ctrl_transport(p.eth_args.ifc, p.eth_args.mac_addr));
        data_transport = transport::sptr(new eth_data_transport(p.eth_args.ifc, p.eth_args.mac_addr, rx_bufsize));
        break;
    case USRP_TYPE_UDP:
        ctrl_transport = transport::sptr(new udp_ctrl_transport(p.udp_args.addr));
        data_transport = transport::sptr(new udp_ctrl_transport(p.udp_args.addr, "32769"));
        //send a dummy data transport so the usrp2 can get the port number
        iovec iov;
        uint32_t nada = 0;
        iov.iov_base = &nada;
        iov.iov_len = sizeof(uint32_t);
        data_transport->sendv(&iov, 1);
        break;
    }

    //pass the transports into a new usrp2 impl
    d_impl = std::auto_ptr<impl>(new usrp2::impl(data_transport, ctrl_transport));
  }
  
  // Public class destructor.  d_impl will auto-delete.
  usrp2::~usrp2()
  {
    // NOP
  }

  // Receive

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
  usrp2::start_rx_streaming(unsigned int items_per_frame, const time_spec_t &time_spec)
  {
    return d_impl->start_rx_streaming(items_per_frame, time_spec);
  }
  
  bool
  usrp2::rx_samples(vrt::rx_packet_handler *handler)
  {
    return d_impl->rx_samples(handler);
  }

  bool
  usrp2::stop_rx_streaming()
  {
    return d_impl->stop_rx_streaming();
  }

  unsigned int
  usrp2::rx_overruns()
  {
    //will probably fix with vrt and usrp2 impl
    return 0;//FIXME d_impl->rx_overruns();
  }
  
  unsigned int
  usrp2::rx_missing()
  {
    //will probably fix with vrt and usrp2 impl
    return 0;//FIXME d_impl->rx_missing();
  }

  // Transmit

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
  usrp2::tx_32fc(
		 const std::complex<float> *samples,
		 size_t nsamples,
		 const vrt::expanded_header *hdr)
  {
    return d_impl->tx_32fc(samples, nsamples, hdr);
  }

  bool
  usrp2::tx_16sc(
		 const std::complex<int16_t> *samples,
		 size_t nsamples,
		 const vrt::expanded_header *hdr)
  {
    return d_impl->tx_16sc(samples, nsamples, hdr);
  }

  bool
  usrp2::tx_raw(
		const uint32_t *items,
		size_t nitems,
		const vrt::expanded_header *hdr)
  {
    return d_impl->tx_raw(items, nitems, hdr);
  }

  // miscellaneous methods

  bool
  usrp2::config_clock(const clock_config_t &clock_config)
  {
    return d_impl->config_clock(clock_config);
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
  usrp2::burn_mac_addr(const u2_mac_addr &new_addr)
  {
    return d_impl->burn_mac_addr(new_addr);
  }

  bool
  usrp2::set_time_at_next_pps(const time_spec_t &time_spec)
  {
    return d_impl->set_time_at_next_pps(time_spec);
  }

  bool
  usrp2::set_time(const time_spec_t &time_spec)
  {
    return d_impl->set_time(time_spec);
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
