/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FCD_SOURCE_C_H
#define INCLUDED_FCD_SOURCE_C_H

#include <gr_hier_block2.h>
#include <gr_audio_source.h>

class fcd_source_c;


typedef boost::shared_ptr<fcd_source_c> fcd_source_c_sptr;


/*!
 * \brief Return a shared_ptr to a new instance of fcd_source_c.
 *
 * This is effectively the public constructor. To avoid accidental use
 * of raw pointers, fcd_source_c's constructor is private.
 * fcd_make_source_c is the public interface for creating new instances.
 */
fcd_source_c_sptr fcd_make_source_c(const std::string device_name = "");

/*! \brief Funcube Dongle source block.
 * 
 * This class provides a Funcube Dongle soure block by wrapping the
 * USB audio interface and the USB HID control interface of the Funcube
 * Dongle into one convenient source block.
 * 
 * The Funcube Dongle needs to have firmware 18f or later for the control
 * interface to work properly. As of early 2011, FCDs still come with firmware
 * 18b. You can use qthid 2.2 (not 3) to upgrade the firmware: http://qthid.sf.net
 */
class fcd_source_c : public gr_hier_block2
{

public:
    fcd_source_c(const std::string device_name = ""); // FIXME: should be private
    ~fcd_source_c();
    
    /*! \brief Set frequency with Hz resolution.
     *  \param freq The frequency in Hz
     * 
     * Set the frequency of the Funcube Dongle with 1 Hz resolution applying
     * the frequency correction set by set_freq_corr().
     * 
     * \see set_freq_khz()
     */
    void set_freq(int freq);

    /*! \brief Set frequency with Hz resolution.
     *  \param freq The frequency in Hz
     * 
     * This is a convenience function that uses float parameter in order to allow
     * using engineering notation in GRC.
     * 
     * \see set_freq_khz()
     */
    void set_freq(float freq);
    
    /*! \brief Set frequency with kHz resolution.
     *  \param freq The frequency in kHz
     * 
     * Sets the frequency of the Funcube Dongle with 1 kHz resolution
     * applying the frequency correction set by set_freq_corr().
     * 
     * \see set_freq()
     */
    void set_freq_khz(int freq);
    
    /*! \brief Set LNA gain.
     *  \param gain The new gain in dB.
     * 
     * Set the LNA gain in the FCD. Valid range is -5 to 30. Although
     * the LNA gain in the FCD takes enumerated values corresponding to
     * 2.5 dB steps, you can can call this method with any float value
     * and it will be rounded to the nearest valid value.
     * 
     * By default the FCD is set to 20 dB and this is a good value for most
     * cases. In noisy areas you may try to reduce the gain.
     */
    void set_lna_gain(float gain);

    /*! \brief Set new frequency correction.
     *  \param ppm The new frequency correction in parts per million
     * 
     * Version 1.1 FCDs (S/N 810 or later) need a correction of -12 ppm.
     * Earlier FCDs need roughly -120 ppm (default for gr-fcd).
     * 
     * Ref: http://www.funcubedongle.com/?p=617
     */
    void set_freq_corr(int ppm);
    
    /*! \brief Set DC offset correction.
     *  \param _dci DC correction for I component (-1.0 to 1.0)
     *  \param _dcq DC correction for Q component (-1.0 to 1.0)
     * 
     * Set DC offset correction in the device. Default is 0.0.
     */
    void set_dc_corr(double _dci, double _dcq);
    
    /*! \brief Set IQ phase and gain balance.
     *  \param _gain The gain correction (-1.0 to 1.0)
     *  \param _phase The phase correction (-1.0 to 1.0)
     * 
     * Set IQ phase and gain balance in the device. The default values
     * are 0.0 for phase and 1.0 for gain.
     */
    void set_iq_corr(double _gain, double _phase);

private:

    audio_source::sptr fcd;  /*!< The audio input source */
    int d_freq_corr;         /*!< The frequency correction in ppm */
    int d_freq_req;          /*!< The latest requested frequency in Hz */
};

#endif /* INCLUDED_FCD_SOURCE_C_H */
