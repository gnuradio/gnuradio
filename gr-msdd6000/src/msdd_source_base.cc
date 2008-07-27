/* -*- c++ -*- */
/*
 * Copyright 2004,2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

//#define MSDD_DEBUG_TRUE
//#define MSDD_DEBUG2_TRUE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <msdd_source_base.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <omnithread.h>
#include <stdexcept>
#include <iostream>
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif


#ifdef MSDD_DEBUG_TRUE
#define MSDD_DEBUG(x) std::cout << x << std::endl;
#else
#define MSDD_DEBUG(x)
#endif


#ifdef MSDD_DEBUG2_TRUE
#define MSDD_DEBUG2(x) std::cout << x << std::endl;
#else
#define MSDD_DEBUG2(x)
#endif


namespace {
	const int OUTPUT_MAX((1 << 15)*8);
	const double PGA_MAX(75);
	const double PGA_MIN(10);
	const double PGA_STEP(.5);
	const double DEFAULT_RX_FREQ(2.417e6);
	const double DEFAULT_GAIN(32);
	const msdd_source_base::msdd_fft_mode_t DEFAULT_FFT_MODE(msdd_source_base::MODE_MAG);
	const msdd_source_base::msdd_fft_points_t DEFAULT_FFT_POINTS(msdd_source_base::S8192);
	const msdd_source_base::msdd_decimation_t DEFAULT_DECIMATION_RATE(msdd_source_base::D2);
}

class msdd_source_base::Impl {
  
public:
  Impl(int opp_mode) :
    d_noverruns (0),
    d_deci_rate (DEFAULT_DECIMATION_RATE),
    d_rx_freq ((unsigned long) DEFAULT_RX_FREQ),
    d_gain(DEFAULT_GAIN),
    d_verbose (false),
    d_updated(false),
    d_msdd_command_type((msdd_command_type_t) opp_mode),
    d_msdd_fft_mode(DEFAULT_FFT_MODE),
    d_desired_sample_size(2^15),
    d_fft_points (DEFAULT_FFT_POINTS)
    {
    
    }

  int            d_noverruns;
  msdd_decimation_t   d_deci_rate;
  unsigned long  d_rx_freq;
  double         d_gain;
  bool           d_verbose;
  bool           d_updated;  
  msdd_command_type_t d_msdd_command_type;
  msdd_fft_mode_t d_msdd_fft_mode;
  unsigned long  d_desired_sample_size;
  
  int            d_socket;        // handle to socket
  int            d_socket_rcv;    // handle to socket retuned in the accept call
  struct in_addr d_ip_src;        // store the source IP address to use
  unsigned short d_port_src;      // the port number to open for connections to this service
  sockaddr_in    d_sockaddr_src;  // store the source sockaddr data (formatted IP address and port number) 
  std::auto_ptr<unsigned char> d_temp_buff;     // hold buffer between calls

  omni_mutex    d_mutex;
  msdd_fft_points_t   d_fft_points; 
  
  struct msdd_request_fft_packet {
      msdd_command_type_t command_type;
      int foo_x20;
      unsigned int center_freq_mhz;
      int offset_freq_hz;
      int gain;
      msdd_fft_window_type_t window_type;
      msdd_fft_points_t fft_points;
      msdd_decimation_t decimation;
      msdd_fft_mode_t fft_mode;
      int number_sets;
  } __attribute__((__packed__));
  
  struct msdd_request_iq_packet {
      msdd_command_type_t command_type;
      int foo0x18;
      unsigned int center_freq_mhz;
      int offset_freq_hz;
      int gain;
      int number;
      msdd_decimation_t decimation;
      int number_sets;
  } __attribute__((__packed__));

  void make_request_fft_packet(msdd_request_fft_packet& packet);
  
  void make_request_iq_packet(msdd_request_iq_packet& packet, unsigned int number_samples);
  
  msdd_request_fft_packet   d_fft_request_packet; // fft request packet
  msdd_request_iq_packet    d_iq_request_packet; // fft request packet
};


msdd_source_base::msdd_source_base (const std::string &name,
				      gr_io_signature_sptr output_signature,
				      int which_board,
				      int opp_mode,
				      const char *src, 
		          	  unsigned short port_src
				      ) throw (std::runtime_error)
  : gr_sync_block (name,
		   gr_make_io_signature (0, 0, 0),
		   output_signature),
		   pimpl( new Impl(opp_mode))
    
{
    int ret (0);
    
    // Set up the address stucture for the source address and port numbers
    // Get the source IP address from the host name
    struct hostent *hsrc (gethostbyname(src));
    
    if(hsrc) {   // if the source was provided as a host namex
      pimpl->d_ip_src = *(struct in_addr*)hsrc->h_addr_list[0];    
    }
    else { // assume it was specified as an IP address
      if((ret=inet_aton(src, &pimpl->d_ip_src)) == 0) {            // format IP address
        perror("Not a valid source IP address or host name");
        throw std::runtime_error("can't initialize source socket");
      }
    }

    pimpl->d_port_src = htons(port_src);     // format port number
    
    pimpl->d_sockaddr_src.sin_family = AF_INET;
    pimpl->d_sockaddr_src.sin_addr   = pimpl->d_ip_src;
    pimpl->d_sockaddr_src.sin_port   = pimpl->d_port_src;

    pimpl->d_temp_buff.reset(new unsigned char[OUTPUT_MAX + 
                                     std::max(sizeof(Impl::msdd_request_iq_packet),
                                         sizeof(Impl::msdd_request_fft_packet))]);   // allow it to hold up to payload_size bytes

  set_output_multiple (OUTPUT_MAX / output_signature->sizeof_stream_item (0));
}
				      

bool
msdd_source_base::open()
{
  omni_mutex_lock l(pimpl->d_mutex);   // hold mutex for duration of this function
  // create socket
  MSDD_DEBUG2("MSDD: Before socket ")
  pimpl->d_socket = socket(PF_INET, SOCK_STREAM, 0);
  if(pimpl->d_socket == -1) {
    perror("socket open");
    throw std::runtime_error("can't open socket");
  }

  // Turn on reuse address
  int opt_val (1);
  if(setsockopt(pimpl->d_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, sizeof(int)) == -1) {
    perror("SO_REUSEADDR");
    throw std::runtime_error("can't set socket option SO_REUSEADDR");
  }

  // Don't wait when shutting down
  linger lngr;
  lngr.l_onoff  = 1;
  lngr.l_linger = 0;
  if(setsockopt(pimpl->d_socket, SOL_SOCKET, SO_LINGER, (void*)&lngr, sizeof(linger)) == -1) {
    perror("SO_LINGER");
    throw std::runtime_error("can't set socket option SO_LINGER");
  }

  // Set a timeout on the receive function to not block indefinitely
  // This value can (and probably should) be changed
//  timeval timeout;
//  timeout.tv_sec = 1;
//  timeout.tv_usec = 0;
//  if(setsockopt(d_socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&timeout, sizeof(timeout)) == -1) {
//    perror("SO_RCVTIMEO");
//    throw std::runtime_error("can't set socket option SO_RCVTIMEO");
//  }

  // bind socket to an address and port number to listen on
  MSDD_DEBUG2("MSDD: Before socket bind to " << pimpl->d_sockaddr_src.sin_port)
  if(::connect(pimpl->d_socket, (struct sockaddr*)&pimpl->d_sockaddr_src, sizeof(pimpl->d_sockaddr_src)) == -1) {
    perror("socket bind");
    throw std::runtime_error("can't bind socket");
  }

  MSDD_DEBUG2("MSDD: Socket open")
  pimpl->d_updated = true;
  return pimpl->d_socket != 0;
}

/* read n bytes from a socket descriptor */
int 
msdd_source_base::readsock(int sockfd, unsigned char* buf, int nbytes) {
    int nleft (nbytes);
    int nread (0);
    
    while (nleft > 0) {
      MSDD_DEBUG2("MSDD: Before socket read: " << nleft)
            if ((nread = ::read(sockfd, buf, nleft)) < 0) {
                return(nread); /* error, nread < 0 */
            } else if (nread == 0) {
                break;
            }
            
            nleft -= nread;
            buf += nread;
    }
    return(nbytes - nleft);
}

bool
msdd_source_base::close()
{
  omni_mutex_lock l(pimpl->d_mutex);   // hold mutex for duration of this function

  if (pimpl->d_socket){
    shutdown(pimpl->d_socket, SHUT_RDWR);
    pimpl->d_socket = 0;
  }
  pimpl->d_updated = true;
  
  return true;
}

msdd_source_base::~msdd_source_base ()
{
  msdd_source_base::close();
}

unsigned int
msdd_source_base::sizeof_basic_sample() const
{
  switch (pimpl->d_msdd_command_type) {
  case SAMPLES_REALTIME:
    return 4;
  case SAMPLES_FFT:
    switch (pimpl->d_msdd_fft_mode) {
    case MODE_IQ:
    case MODE_MAG:
      return 4;
    case MODE_MAGDB:
      return 1;
    default:
      assert (false); // bad mode
    }
  default:
    assert (false); // bad mode
  }
}

bool
msdd_source_base::start()
{
	return msdd_source_base::open();
}

bool
msdd_source_base::stop()
{
	return msdd_source_base::close();
}

void* 
msdd_source_base::make_request_packet(unsigned int& size, unsigned int number_samples) {
  switch (pimpl->d_msdd_command_type) {
  case SAMPLES_REALTIME:
    pimpl->make_request_iq_packet(pimpl->d_iq_request_packet, number_samples);
    size = sizeof (pimpl->d_iq_request_packet);
    return &pimpl->d_iq_request_packet;
  case SAMPLES_FFT:
    pimpl->make_request_fft_packet(pimpl->d_fft_request_packet);
    size = sizeof (pimpl->d_fft_request_packet);
    return &pimpl->d_fft_request_packet;
  default:
    assert (false); // bad mode
  }
}

void 
msdd_source_base::Impl::make_request_fft_packet(msdd_request_fft_packet& packet) 
{
    packet.command_type = SAMPLES_FFT;  // FFT samples Command
    packet.foo_x20 = 0x20;
    packet.center_freq_mhz = d_rx_freq;
    packet.offset_freq_hz = 0;
    packet.gain = (int) d_gain; // gain
    packet.window_type = WINDOW_HANNING; // magic number
    packet.fft_points = d_fft_points;
    packet.decimation = d_deci_rate;
    packet.fft_mode = MODE_MAGDB;
    packet.number_sets = 1;
}

void 
msdd_source_base::Impl::make_request_iq_packet(msdd_request_iq_packet& packet, unsigned int number_samples) 
{
    packet.command_type = SAMPLES_REALTIME;  // FFT samples Command
    packet.foo0x18 = 0x18; // magic number
    packet.center_freq_mhz = d_rx_freq;
    packet.offset_freq_hz = 0;
    packet.gain = (int) d_gain; // gain
    packet.number = number_samples * 4;
    packet.decimation = d_deci_rate;
    packet.number_sets = 1;
}

int
msdd_source_base::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  int output_index (0);
  int output_items_produced;
  int bytes_read;

  unsigned int packet_size;
  
  MSDD_DEBUG("MSDD: requested items: " << noutput_items)
  int noutput_items_desired = std::min (noutput_items, (int) pimpl->d_desired_sample_size);
  MSDD_DEBUG("MSDD: desired items: " << noutput_items_desired)
  
  while (output_index < noutput_items_desired){

    int nbytes = (pimpl->d_msdd_command_type == SAMPLES_REALTIME) ? 
        ninput_bytes_reqd_for_noutput_items (noutput_items_desired - output_index) :
        ninput_bytes_reqd_for_noutput_items (msdd_source_base::fft_points());
    
    void* request_packet = msdd_source_base::make_request_packet(packet_size, noutput_items_desired);
    
    nbytes = std::min (nbytes, OUTPUT_MAX);
    MSDD_DEBUG2("MSDD: payload sizes: nbytes1: " << nbytes )

    // send request
    int result_nbytes = ::write(pimpl->d_socket, request_packet, packet_size);
    //assert (result_nbytes == sizeof(msdd_request_packet));
    
    // receive ack
    result_nbytes = ::read (pimpl->d_socket, (unsigned char*) request_packet, packet_size);
    MSDD_DEBUG2("MSDD: response: " << result_nbytes)
    //assert (result_nbytes == sizeof(msdd_request_packet));
    
    // receive payload
    result_nbytes = msdd_source_base::readsock (pimpl->d_socket, pimpl->d_temp_buff.get(), nbytes);
    MSDD_DEBUG("MSDD: reading bytes: " << nbytes << " received: " << result_nbytes)
    if (result_nbytes > (int) nbytes){
      // fprintf (stderr, "msdd_source: overrun\n");
      fputs ("uO", stderr);
      pimpl->d_noverruns++;
      result_nbytes = nbytes; // truncate
    }
    
    if (result_nbytes < 0)	// We've got a problem.  Usually board unplugged or powered down.
      return -1;		// Indicate we're done.

    if (result_nbytes != nbytes){	// not really an error, but unexpected
      fprintf (stderr, "msdd_source: short read.  Expected %d, got %d\n",
	       nbytes, result_nbytes);
    }

    copy_from_msdd_buffer (output_items,
			   output_index,
			   noutput_items_desired - output_index,   // output_items_available
			   output_items_produced,	   // [out]
			   pimpl->d_temp_buff.get(),				   // usrp_buffer
			   result_nbytes,
			   bytes_read);			   // [out]
    
    output_index += output_items_produced;
    
    if (pimpl->d_msdd_command_type == SAMPLES_FFT) break; 
  }

  MSDD_DEBUG("MSDD: items produced: " << output_items_produced << " index: " << output_index)
  
  //assert(false);
  return output_index;
}


bool
msdd_source_base::set_decim_rate (unsigned int rate)
{
	bool result (true);
	switch (rate) {
    case 1:
      pimpl->d_deci_rate = D0;
      break;
    case 2:
      pimpl->d_deci_rate = D1;
      break;      
	case 4:
	  pimpl->d_deci_rate = D2;
      break;	  
	case 8:
	  pimpl->d_deci_rate = D3;
      break;	  
	case 16:
	  pimpl->d_deci_rate = D4;
	  break;
    case 32:
      pimpl->d_deci_rate = D5;
      break;
    case 64:
      pimpl->d_deci_rate = D6;
      break;
    case 128:
      pimpl->d_deci_rate = D7;
      break;
    case 256:
      pimpl->d_deci_rate = D8;
      break;
	default:
	  result = false;
	}
	
	return result;
}
//
//bool
//msdd_source_base::set_nchannels (int nchan)
//{
//  // return d_usrp->set_nchannels (nchan);
//	return true;
//}
//
//bool
//msdd_source_base::set_mux (int mux)
//{
//  return d_usrp->set_mux (mux);
//}

bool
msdd_source_base::set_rx_freq (int channel, double freq)
{
	assert (channel == 0);
	bool result (false);
	
	if (freq >=  30e6 && freq <= 6e9) {
	  pimpl->d_rx_freq = (unsigned long) freq / 1000000;
		result = true;
	}
	
	return result;
}


unsigned long
msdd_source_base::set_fft_size (int channel, unsigned long fft_size)
{
    assert (channel == 1);
    
    switch (fft_size) {
      case 256:        
        pimpl->d_fft_points = S256;
        break;
      case 512:
        pimpl->d_fft_points = S512;       
        break;
      case 1024:
        pimpl->d_fft_points = S1024;
        break;        
      case 2048:
        pimpl->d_fft_points = S2048;
        break;        
      case 4096:
        pimpl->d_fft_points = S4096;
        break;        
      case 8192:
        pimpl->d_fft_points = S8192;
        break;        
      case 16384:
        pimpl->d_fft_points = S16384;
        break;        
      case 32768:
        pimpl->d_fft_points = S32768;
        break;        
    }
    
    return msdd_source_base::fft_points();
}

//
//long
//msdd_source_base::fpga_master_clock_freq() const
//{
//  return d_usrp->fpga_master_clock_freq();
//}
//
//long
//msdd_source_base::converter_rate() const
//{
//  // return d_usrp->converter_rate();
//	return 8;
//}

unsigned int
msdd_source_base::decim_rate () const
{
	return 1 << pimpl->d_deci_rate;
}
//
//int
//msdd_source_base::nchannels () const
//{
//  return d_usrp->nchannels ();
//}
//
//int
//msdd_source_base::mux () const
//{
//  return d_usrp->mux ();
//}

double
msdd_source_base::rx_freq (int channel) const
{
  assert (channel == 0);
	
  return pimpl->d_rx_freq;
}

unsigned int
msdd_source_base::fft_points() const
{
  return (1 << pimpl->d_fft_points);
}

int 
msdd_source_base::noverruns () const 
{ 
  return pimpl->d_noverruns; 
}

//bool
//msdd_source_base::set_fpga_mode (int mode)
//{
//  return d_usrp->set_fpga_mode (mode);
//}
//
//bool
//msdd_source_base::set_ddc_phase (int channel, int phase)
//{
//  return d_usrp->set_ddc_phase(channel, phase);
//}
//
//bool
//msdd_source_base::set_dc_offset_cl_enable(int bits, int mask)
//{
//  return d_usrp->set_dc_offset_cl_enable(bits, mask);
//}

void
msdd_source_base::set_verbose (bool verbose)
{  
  pimpl->d_verbose = verbose;
}
//
//bool
//msdd_source_base::write_aux_dac (int which_dboard, int which_dac, int value)
//{
//  return d_usrp->write_aux_dac (which_dboard, which_dac, value);
//}
//
//int
//msdd_source_base::read_aux_adc (int which_dboard, int which_adc)
//{
//  return d_usrp->read_aux_adc (which_dboard, which_adc);
//}
//
//bool
//msdd_source_base::write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf)
//{
//  return d_usrp->write_eeprom (i2c_addr, eeprom_offset, buf);
//}
//
//std::string
//msdd_source_base::read_eeprom (int i2c_addr, int eeprom_offset, int len)
//{
//  return d_usrp->read_eeprom (i2c_addr, eeprom_offset, len);
//}
//
//bool
//msdd_source_base::write_i2c (int i2c_addr, const std::string buf)
//{
//  return d_usrp->write_i2c (i2c_addr, buf);
//}
//
//std::string
//msdd_source_base::read_i2c (int i2c_addr, int len)
//{
//  return d_usrp->read_i2c (i2c_addr, len);
//}
//
bool
msdd_source_base::set_pga (int which, double gain)
{
	if (gain >= PGA_MIN & gain <= PGA_MAX) {
	  pimpl->d_gain = gain;
		return true;
	}
	return false;
}

double
msdd_source_base::pga (int which) const
{
  return pimpl->d_gain;
}

double
msdd_source_base::pga_min () const
{
  return PGA_MIN;
}

double
msdd_source_base::pga_max () const
{
  return PGA_MAX;
}

double
msdd_source_base::pga_db_per_step () const
{
  return PGA_STEP;
}

//int
//msdd_source_base::daughterboard_id (int which) const
//{
//  return d_usrp->daughterboard_id (which);
//}
//
//
//bool
//msdd_source_base::set_adc_offset (int which, int offset)
//{
//  return d_usrp->set_adc_offset (which, offset);
//}
//
//bool
//msdd_source_base::set_dac_offset (int which, int offset, int offset_pin)
//{
//  return d_usrp->set_dac_offset (which, offset, offset_pin);
//}
//
//bool
//msdd_source_base::set_adc_buffer_bypass (int which, bool bypass)
//{
//  return d_usrp->set_adc_buffer_bypass (which, bypass);
//}

std::string
msdd_source_base::serial_number()
{
  return "SoftTronics MSDD 6000";
}
//
//bool
//msdd_source_base::_write_oe (int which_dboard, int value, int mask)
//{
//  return d_usrp->_write_oe (which_dboard, value, mask);
//}
//
//bool
//msdd_source_base::write_io (int which_dboard, int value, int mask)
//{
//  return d_usrp->write_io (which_dboard, value, mask);
//}
//
//int
//msdd_source_base::read_io (int which_dboard)
//{
//  return d_usrp->read_io (which_dboard);
//}
//
//
//
//
//// internal routines...
//
//bool
//msdd_source_base::_write_fpga_reg (int regno, int value)
//{
//  return d_usrp->_write_fpga_reg (regno, value);
//}
//
//bool
//msdd_source_base::_write_fpga_reg_masked (int regno, int value, int mask)
//{
//  return d_usrp->_write_fpga_reg_masked (regno, value, mask);
//}
//
//int
//msdd_source_base::_read_fpga_reg (int regno)
//{
//  return d_usrp->_read_fpga_reg (regno);
//}
//
//bool
//msdd_source_base::_write_9862 (int which_codec, int regno, unsigned char value)
//{
//  return d_usrp->_write_9862 (which_codec, regno, value);
//}
//
//int
//msdd_source_base::_read_9862 (int which_codec, int regno) const
//{
//  return d_usrp->_read_9862 (which_codec, regno);
//}
//
//bool
//msdd_source_base::_write_spi (int optional_header, int enables,
//			       int format, std::string buf)
//{
//  return d_usrp->_write_spi (optional_header, enables, format, buf);
//}
//
//std::string
//msdd_source_base::_read_spi (int optional_header, int enables, int format, int len)
//{
//  return d_usrp->_read_spi (optional_header, enables, format, len);
//}
//
//bool
//msdd_source_base::set_format(unsigned int format)
//{
//  return d_usrp->set_format(format);
//}
//
//unsigned int
//msdd_source_base::format() const
//{
//  return d_usrp->format();
//}
//
//unsigned int
//msdd_source_base::make_format(int width, int shift, bool want_q, bool bypass_halfband)
//{
//  return usrp_standard_rx::make_format(width, shift, want_q, bypass_halfband);
//}
//
//int
//msdd_source_base::format_width(unsigned int format)
//{
//  return usrp_standard_rx::format_width(format);
//}
//
//int
//msdd_source_base::format_shift(unsigned int format)
//{
//  return usrp_standard_rx::format_shift(format);
//}
//
//bool
//msdd_source_base::format_want_q(unsigned int format)
//{
//  return usrp_standard_rx::format_want_q(format);
//}
//
//bool
//msdd_source_base::format_bypass_halfband(unsigned int format)
//{
//  return usrp_standard_rx::format_bypass_halfband(format);
//}

bool msdd_source_base::set_desired_packet_size (int which, unsigned long packet_size) {
  bool result(false);
  
  if (pimpl->d_desired_sample_size < 2^32) { // FIXME: find maximum sample request for MSDD check if greater than 
    pimpl->d_desired_sample_size = packet_size;
  }
  return result;
}

unsigned long msdd_source_base::desired_packet_size (int which) const {
  return pimpl->d_desired_sample_size;
}
