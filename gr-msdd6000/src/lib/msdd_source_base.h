/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MSDD_SOURCE_BASE_H
#define INCLUDED_MSDD_SOURCE_BASE_H
#include <gr_sync_block.h>
#include <stdexcept>

/*!
 * \brief abstract interface to MSDD 6000 Softronics module Rx path (Rev 1)
 */

class msdd_source_base : public gr_sync_block {
public: 
  enum msdd_command_type_t {
    COMMAND_STATUS = 0,
    SAMPLES_REALTIME = 1,
    SAMPLES_FFT = 2,
  };
  
  enum msdd_fft_window_type_t {
    WINDOW_RECTANGLE = 0,
    WINDOW_HANNING = 1,
    WINDOW_HAMMING = 2,
    WINDOW_BLACKMAN = 3
  };
  
  enum msdd_fft_mode_t {
    MODE_IQ=0, 
    MODE_MAG=1, 
    MODE_MAGDB=2
  };
  
  enum msdd_decimation_t {
    D0=0, 
    D1=1, 
    D2=2, 
    D3=3, 
    D4=4, 
    D5=5, 
    D6=6, 
    D7=7, 
    D8=8
  };    
  
  enum msdd_fft_points_t {
    S256=8, 
    S512=9, 
    S1024=10, 
    S2048=11, 
    S4096=12, 
    S8192=13, 
    S16384=14, 
    S32768=15
  };
  
private:
   
  class Impl;
  friend class Impl;
  std::auto_ptr<Impl> pimpl;

protected:
   
  msdd_source_base (const std::string &name,
		    gr_io_signature_sptr output_signature,
		    int which_board,
		    int opp_mode,
		    const char *src, 
		    unsigned short port_src
		    ) throw (std::runtime_error);

  /*!
   * \brief return number of msdd input bytes required to produce noutput items.
   */
  virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items) = 0;

  /*!
   * \brief number of bytes in a low-level sample
   */
  unsigned int sizeof_basic_sample() const;

  virtual void copy_from_msdd_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *msdd_buffer,
				      int msdd_buffer_length,
				      int &bytes_read) = 0;
  
  int readsock(int sockfd, unsigned char* buf, int nbytes);
  
  void  write_request_packet(unsigned int number_samples);
  void* make_request_packet(unsigned int& size, unsigned int number_samples);
  
  unsigned long set_fft_size (int channel, unsigned long fft_size);
  
public:
  //! magic value used on alternate register read interfaces
  static const int READ_FAILED = -99999;
  
  ~msdd_source_base ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
  
  bool start();
  bool stop();

  /*!
   * \brief open a socket specified by the port and ip address info
   *
   * Opens a socket, binds to the address, and waits for a connection
   * over UDP. If any of these fail, the fuction retuns the error and exits.
   */
  bool open();

  /*!
   * \brief Close current socket.
   *
   * Shuts down read/write on the socket
   */
  bool close();
  
  /*!
   * \brief Set decimator rate.  \p rate must be EVEN and in [8, 256].
   *
   * The final complex sample rate across the USB is
   *   adc_freq () / decim_rate ()
   */
  bool set_decim_rate (unsigned int rate);
  //bool set_nchannels (int nchan);
  //bool set_mux (int mux);

  /*!
   * \brief set the center frequency of the digital down converter.
   *
   * \p channel must be 0.  \p freq is the center frequency in Hz.
   * It must be in the range [-FIXME, FIXME].  The frequency specified is
   * quantized.  Use rx_freq to retrieve the actual value used.
   */
  bool set_rx_freq (int channel, double freq);

  /*!
   * \brief
   */
  bool set_opp_mode (int channel, msdd_command_type_t mode);
  
//
//  /*!
//   * \brief set fpga special modes
//   */
//  bool set_fpga_mode (int mode);

  void set_verbose (bool verbose);
//
//  /*!
//   * \brief Set the digital down converter phase register.
//   *
//   * \param channel	which ddc channel [0, 3]
//   * \param phase	32-bit integer phase value.
//   */
//  bool set_ddc_phase(int channel, int phase);
//
  /*!
   * \brief Set Programmable Gain Amplifier (PGA)
   *
   * \param which	which A/D [0,3]
   * \param gain_in_db	gain value (linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool set_pga (int which, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain setting in dB.
   *
   * \param which	which A/D [0,3]
   */
  double pga (int which) const;

  /*!
   * \brief Return minimum legal PGA setting in dB.
   */
  double pga_min () const;

  /*!
   * \brief Return maximum legal PGA setting in dB.
   */
  double pga_max () const;

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const;

  // ACCESSORS

//  long converter_rate() const;

  unsigned int decim_rate () const;
//  int nchannels () const;
//  int mux () const;
  double rx_freq (int channel) const;
  unsigned int fft_points() const;
  int noverruns () const;

  /*!
   * \brief return the msdd's serial number.
   *
   * \returns non-zero length string iff successful.
   */
  std::string serial_number();

//  /*!
//   * \brief Enable/disable automatic DC offset removal control loop in FPGA
//   *
//   * \param bits  which control loops to enable
//   * \param mask  which \p bits to pay attention to
//   *
//   * If the corresponding bit is set, enable the automatic DC
//   * offset correction control loop.
//   *
//   * <pre>
//   * The 4 low bits are significant:
//   *
//   *   ADC0 = (1 << 0)
//   *   ADC1 = (1 << 1)
//   *   ADC2 = (1 << 2)
//   *   ADC3 = (1 << 3)
//   * </pre>
//   *
//   * By default the control loop is enabled on all ADC's.
//   */
//  bool set_dc_offset_cl_enable(int bits, int mask);

  /*!
   * \brief Specify Rx data format.
   *
   * \param format	format specifier
   *
   * Rx data format control register
   *
   *     3                   2                   1                       
   *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   *  +-----------------------------------------+-+-+---------+-------+
   *  |          Reserved (Must be zero)        |B|Q|  WIDTH  | SHIFT |
   *  +-----------------------------------------+-+-+---------+-------+
   *
   *  SHIFT specifies arithmetic right shift [0, 15]
   *  WIDTH specifies bit-width of I & Q samples across the USB [1, 16] (not all valid)
   *  Q     if set deliver both I & Q, else just I
   *  B     if set bypass half-band filter.
   *
   * Right now the acceptable values are:
   *
   *   B  Q  WIDTH  SHIFT
   *   0  1    16     0
   *   0  1     8     8
   *
   * More valid combos to come.
   *
   * Default value is 0x00000300  16-bits, 0 shift, deliver both I & Q.
   */
//  bool set_format(unsigned int format);

  /*!
   * \brief return current format
   */
//  unsigned int format () const;
//
//  static unsigned int make_format(int width=16, int shift=0,
//				  bool want_q=true, bool bypass_halfband=false);
//  static int format_width(unsigned int format);
//  static int format_shift(unsigned int format);
//  static bool format_want_q(unsigned int format);
//  static bool format_bypass_halfband(unsigned int format);

    bool set_desired_packet_size (int which, unsigned long packet_size);

    unsigned long desired_packet_size (int which) const;
};

#endif /* INCLUDED_MSDD_SOURCE_BASE_H */
