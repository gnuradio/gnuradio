/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"				// the common stuff

%{

#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "msdd_source_s.h"
#include "msdd_source_c.h"
#include "msdd_source_simple.h"
#include <stdexcept>
%}

// ================================================================
//			   abstract classes
// ================================================================

// ----------------------------------------------------------------

class msdd_source_base : public gr_sync_block {

 protected:
  msdd_source_base (const std::string &name,
      gr_io_signature_sptr output_signature,
      int which_board,
      msdd_source_base::msdd_command_type_t opp_mode,
      const char *src, 
      unsigned short port_src
         ) throw (std::runtime_error);

  /*!
   * \brief return number of msdd input bytes required to produce noutput items.
   */
  int ninput_bytes_reqd_for_noutput_items (int noutput_items) = 0;

  /*!
   * \brief number of bytes in a low-level sample
   */
  unsigned int sizeof_basic_sample() const;

  /*!
   * \brief convert between native msdd format and output item format
   *
   * \param output_items[out]   stream(s) of output items
   * \param output_index[in]    starting index in output_items
   * \param output_items_available[in]  number of empty items available at item[index]
   * \param output_items_produced[out]  number of items produced by copy
   * \param msdd_buffer[in]   source buffer
   * \param msdd_buffer_length[in]  number of bytes available in \p msdd_buffer
   * \param bytes_read[out]   number of bytes read from \p msdd_buffer
   *
   * The copy must consume all bytes available.  That is, \p bytes_read must equal
   * \p msdd_buffer_length.
   */
  virtual void copy_from_msdd_buffer (gr_vector_void_star &output_items,
              int output_index,
              int output_items_available,
              int &output_items_produced,
              const void *msdd_buffer,
              int msdd_buffer_length,
              int &bytes_read) = 0;
  
  int readsock(int sockfd, unsigned char* buf, int nbytes);

  
  void* make_request_packet(unsigned int& size, unsigned int number_samples);
  
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
   * \brief Set Programmable Gain Amplifier (PGA)
   *
   * \param which	which D/A [0,3]
   * \param gain_in_db	gain value (linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   * Note that DAC 0 and DAC 1 share a gain setting as do DAC 2 and DAC 3.
   * Setting DAC 0 affects DAC 1 and vice versa.  Same with DAC 2 and DAC 3.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool set_pga (int which, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain in dB.
   *
   * \param which	which D/A [0,3]
   */
  double pga (int which) const;

  /*!
   * \brief Return minimum legal PGA gain in dB.
   */
  double pga_min () const;

  /*!
   * \brief Return maximum legal PGA gain in dB.
   */
  double pga_max () const;

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const;

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

  /*!
   * \brief set the center frequency of the digital down converter.
   *
   * \p channel must be 0.  \p freq is the center frequency in Hz.
   * It must be in the range [-FIXME, FIXME].  The frequency specified is
   * quantized.  Use rx_freq to retrieve the actual value used.
   */
  bool set_rx_freq (int channel, double freq);

  void set_verbose (bool verbose);

  // ACCESSORS

  unsigned int decim_rate () const;
  double rx_freq (int channel) const;
  int noverruns () const { return d_noverruns; }

  /*!
   * \brief return the msdd's serial number.
   *
   * \returns non-zero length string iff successful.
   */
  std::string serial_number();
  
  bool set_desired_packet_size (int which, unsigned long packet_size);

  unsigned long desired_packet_size (int which) const;

};


// ================================================================
//      concrete sources
// ================================================================


// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(msdd,source_s)

msdd_source_s_sptr
msdd_make_source_s (int which_board, 
            unsigned int decim_rate,
            unsigned int fft_points,
            double initial_rx_freq,
            int opp_mode,
            const char *src, 
            unsigned short port_src
         ) throw (std::runtime_error);


class msdd_source_s : public msdd_source_base {
 protected:
  msdd_source_s (int which_board, 
      unsigned int decim_rate,
      unsigned int fft_points,
      double initial_rx_freq,
      int opp_mode,
          const char *src, 
          unsigned short port_src
      ) throw (std::runtime_error);

 virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items);
 
 public:
  ~msdd_source_s ();
};


GR_SWIG_BLOCK_MAGIC(msdd,source_c)

msdd_source_c_sptr
msdd_make_source_c (int which_board, 
            int opp_mode,
            const char *src, 
            unsigned short port_src
         ) throw (std::runtime_error);


class msdd_source_c : public msdd_source_base {
 protected:
  msdd_source_c (int which_board, 
      int opp_mode,
          const char *src, 
          unsigned short port_src
      ) throw (std::runtime_error);

 virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items);
 
 public:
  ~msdd_source_c ();
};





GR_SWIG_BLOCK_MAGIC(msdd,source_simple)

msdd_source_simple_sptr
msdd_make_source_simple (
	const char *src,
	unsigned short port_src
	);

class msdd_source_simple : public gr_sync_block {
  protected:
    msdd_source_simple(
	const char *src,
	unsigned short port_src
	);
 
  public:
    ~msdd_source_c(); 
  int ninput_bytes_reqd_for_noutput_items (int noutput_items) = 0;
  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);

  bool start();
  bool stop();

  long adc_freq();
  int decim_rate();
  gr_vector_int gain_range();
  gr_vector_float freq_range();

  bool set_decim_rate(unsigned int);
  bool set_rx_freq(int,double);
  bool set_pga(int,double);
  

  };
