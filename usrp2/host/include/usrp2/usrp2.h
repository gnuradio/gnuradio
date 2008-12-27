/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP2_H
#define INCLUDED_USRP2_H

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <vector>
#include <complex>
#include <usrp2/rx_sample_handler.h>
#include <usrp2/tune_result.h>


/*
 * N.B., The interfaces described here are still in flux.
 *
 * We will keep all the code in the tree up-to-date with regard to changes
 * here, but reserve the right to change this on a whim.
 */

namespace usrp2 {

  /*!
   * Structure to hold properties of USRP2 hardware devices.
   *
   */
  struct props
  {
    std::string addr;
    uint16_t hw_rev;
    uint8_t fpga_md5sum[16];
    uint8_t sw_md5sum[16];
  };

  typedef std::vector<props> props_vector_t;

  /*!
   * \brief Search the ethernet for all USRP2s or for a specific USRP2.
   *
   * \param ifc is the name of the OS ethernet interface (e.g., "eth0")
   * \param mac_addr is the MAC address of the desired USRP2, or "" to search for all.
   * mac_addr must be either a zero length string, "", or must be of the form
   * "01:02:03:04:05:06" or "05:06".
   *
   * \returns a vector of properties, 1 entry for each matching USRP2 found.
   */
  props_vector_t find(const std::string &ifc, const std::string &mac_addr=""); 

  class tune_result;
  
  class usrp2 : boost::noncopyable
  {
  public:
    static const unsigned int MAX_CHAN = 30;

    /*!
     * Shared pointer to this class
     */ 
    typedef boost::shared_ptr<usrp2> sptr;
 
    /*! 
     * Static function to return an instance of usrp2 as a shared pointer
     *
     * \param ifc   Network interface name, e.g., "eth0"
     * \param addr  Network mac address, e.g., "01:23:45:67:89:ab", "89:ab" or "".
     *              If \p addr is HH:HH, it's treated as if it were 00:50:c2:85:HH:HH
     *              "" will autoselect a USRP2 if there is only a single one on the local ethernet.
     */
    static sptr make(const std::string &ifc, const std::string &addr="");

    /*!
     * Class destructor
     */
    ~usrp2();  

    /*!
     * Returns the MAC address associated with this USRP
     */
    std::string mac_addr();

    /*
     * ----------------------------------------------------------------
     * Rx configuration and control
     * ----------------------------------------------------------------
     */

    /*!
     * Set receiver gain
     * \param gain in dB (more or less)
     */
    bool set_rx_gain(double gain);

    //! return minimum Rx gain 
    double rx_gain_min();

    //! return maximum Rx gain 
    double rx_gain_max();

    //! return Rx gain db_per_step
    double rx_gain_db_per_step();

    /*!
     * Set receiver center frequency
     */
    bool set_rx_center_freq(double frequency, tune_result *result);

    //! return minimum Rx center frequency
    double rx_freq_min();

    //! return maximum Rx center frequency
    double rx_freq_max();

    /*!
     * Set receiver sample rate decimation
     */
    bool set_rx_decim(int decimation_factor);

    //! Return current decimation factor
    int rx_decim();

    /*!
     * Set receiver IQ magnitude scaling
     */
    bool set_rx_scale_iq(int scale_i, int scale_q);

    /*!
     * Set received sample format
     *   
     *    domain: complex or real
     *      type: floating, fixed point, or raw
     *     depth: bits per sample
     *
     * Sets format over the wire for samples from USRP2.
     */
    // bool set_rx_format(...);

    /*!
     * Start streaming receive mode.  USRP2 will send a continuous stream of
     * DSP pipeline samples to host.  Call rx_samples(...) to access.
     * 
     * \param channel          Stream channel number (0-30)
     * \param items_per_frame  Number of 32-bit items per frame.
     */
    bool start_rx_streaming(unsigned int channel=0, unsigned int items_per_frame=0);
  
    /*!
     * Stop streaming receive mode.
     */
    bool stop_rx_streaming(unsigned int channel=0);

    /*!
     * \brief Receive data from the specified channel
     * This method is used to receive all data: streaming or discrete.
     */
    bool rx_samples(unsigned int channel, rx_sample_handler *handler);

    /*!
     * Returns number of times receive overruns have occurred
     */
    unsigned int rx_overruns();
    
    /*!
     * Returns total number of missing frames from overruns.
     */
    unsigned int rx_missing();

    /*
     * ----------------------------------------------------------------
     * Tx configuration and control
     * ----------------------------------------------------------------
     */

    /*!
     * Set transmitter gain
     */
    bool set_tx_gain(double gain);

    //! return minimum Tx gain 
    double tx_gain_min();

    //! return maximum Tx gain 
    double tx_gain_max();

    //! return Tx gain db_per_step
    double tx_gain_db_per_step();

    /*!
     * Set transmitter center frequency
     */
    bool set_tx_center_freq(double frequency, tune_result *result);

    //! return minimum Tx center frequency
    double tx_freq_min();

    //! return maximum Tx center frequency
    double tx_freq_max();

    /*!
     * Set transmitter sample rate interpolation
     */
    bool set_tx_interp(int interpolation_factor);

    //! Return current interpolation factor
    int tx_interp();

    /*!
     * Set transmit IQ magnitude scaling
     */
    bool set_tx_scale_iq(int scale_i, int scale_q);

    /*!
     * Set transmit sample format
     *   
     *    domain: complex or real
     *      type: floating, fixed point, or raw
     *     depth: bits per sample
     *
     * Sets format over the wire for samples to USRP2.
     */
    // bool set_tx_format(...);

    /*!
     * \brief transmit complex<float> samples to USRP2
     *
     * \param channel specifies the channel to send them to
     * \param samples are the samples to transmit.  They should be in the range [-1.0, +1.0]
     * \param nsamples is the number of samples to transmit
     * \param metadata provides the timestamp and flags
     *
     * The complex<float> samples are converted to the appropriate 
     * "on the wire" representation, depending on the current USRP2
     * configuration.  Typically, this is big-endian 16-bit I & Q.
     */
    bool tx_32fc(unsigned int channel,
		 const std::complex<float> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata);

    /*!
     * \brief transmit complex<int16_t> samples to USRP2
     *
     * \param channel specifies the channel to send them to
     * \param samples are the samples to transmit
     * \param nsamples is the number of samples to transmit
     * \param metadata provides the timestamp and flags
     *
     * The complex<int16_t> samples are converted to the appropriate
     * "on the wire" representation, depending on the current USRP2
     * configuration.  Typically, this is big-endian 16-bit I & Q.
     */
    bool tx_16sc(unsigned int channel,
		 const std::complex<int16_t> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata);

    /*!
     * \brief transmit raw uint32_t data items to USRP2
     *
     * The caller is responsible for ensuring that the items are
     * formatted appropriately for the USRP2 and its configuration.
     * This method is used primarily by the system itself.  Users
     * should call tx_32fc or tx_16sc instead.
     *
     * \param channel specifies the channel to send them to
     * \param items are the data items to transmit
     * \param nitems is the number of items to transmit
     * \param metadata provides the timestamp and flags
     */
    bool tx_raw(unsigned int channel,
		const uint32_t *items,
		size_t nitems,
		const tx_metadata *metadata);

    /*
     * ----------------------------------------------------------------
     *  miscellaneous methods
     * ----------------------------------------------------------------
     */

    /*!
     * \brief MIMO configuration
     *
     * \param flags from usrp2_mimo_config.h
     *
     * <pre>
     *   one of these:
     *
     *     MC_WE_DONT_LOCK
     *     MC_WE_LOCK_TO_SMA
     *     MC_WE_LOCK_TO_MIMO
     *
     *   and optionally this:
     *
     *     MC_PROVIDE_CLK_TO_MIMO
     * </pre>
     */
    bool config_mimo(int flags);


    //! Get frequency of master oscillator in Hz
    bool fpga_master_clock_freq(long *freq);

    // Get Sampling rate of A/D converter in Hz
    bool adc_rate(long *rate);

    // Get Sampling rate of D/A converter in Hz
    bool dac_rate(long *rate);

    /*!
     * \brief Get Tx daughterboard ID
     *
     * \param[out] dbid returns the daughterboard id.
     *
     * daughterboard id >= 0 if successful, -1 if no daugherboard installed,
     * -2 if invalid EEPROM on daughterboard.
     */
    bool tx_daughterboard_id(int *dbid);

    /*!
     * \brief Get Rx daughterboard ID
     *
     * \param[out] dbid returns the daughterboard id.
     *
     * daughterboard id >= 0 if successful, -1 if no daugherboard installed,
     * -2 if invalid EEPROM on daughterboard.
     */
    bool rx_daughterboard_id(int *dbid);

    /*
     * ----------------------------------------------------------------
     *  Low level methods
     * ----------------------------------------------------------------
     */

    /*!
     * Burn new mac address into EEPROM on USRP2
     *
     * \param new_addr  Network mac address, e.g., "01:23:45:67:89:ab" or "89:ab".
     *                  If \p addr is HH:HH, it's treated as if it were 00:50:c2:85:HH:HH
     */
    bool burn_mac_addr(const std::string &new_addr);

    /*!
     * Reset master time to 0 at next PPS rising edge
     */
    bool sync_to_pps();

    /*!
     * Read memory from Wishbone bus
     *
     * \param addr      32-bit aligned address.  Only the lower 16-bits are significant.
     * \param len       Number of bytes to read, must be positive and multiple of 4.
     * 
     * \returns         Vector of 8-bit read values
     *
     * WARNING: Attempts to read memory from addresses that do not correspond to RAM or
     * memory-mapped peripherals may cause the USRP2 to hang, requiring a power cycle.
     * 
     */
    std::vector<uint8_t> peek(uint32_t addr, uint32_t len);


#if 0	// not yet implemented
    /*!
     * \brief Write EEPROM on motherboard or any daughterboard.
     * \param i2c_addr		I2C bus address of EEPROM
     * \param eeprom_offset	byte offset in EEPROM to begin writing
     * \param buf		the data to write
     * \returns true iff sucessful
     */
    bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string &buf);

    /*!
     * \brief Read EEPROM on motherboard or any daughterboard.
     * \param i2c_addr		I2C bus address of EEPROM
     * \param eeprom_offset	byte offset in EEPROM to begin reading
     * \param len		number of bytes to read
     * \returns the data read if successful, else a zero length string.
     */
    std::string read_eeprom (int i2c_addr, int eeprom_offset, int len);

    /*!
     * \brief Write to I2C peripheral
     * \param i2c_addr		I2C bus address (7-bits)
     * \param buf		the data to write
     * \returns true iff successful
     * Writes are limited to a maximum of of 64 bytes.
     */
    bool write_i2c (int i2c_addr, const std::string &buf);

    /*!
     * \brief Read from I2C peripheral
     * \param i2c_addr		I2C bus address (7-bits)
     * \param len		number of bytes to read
     * \returns the data read if successful, else a zero length string.
     * Reads are limited to a maximum of 64 bytes.
     */
    std::string read_i2c (int i2c_addr, int len);

    /*!
     * \brief Write data to SPI bus peripheral.
     *
     * \param optional_header	0,1 or 2 bytes to write before buf.
     * \param enables		bitmask of peripherals to write. See usrp_spi_defs.h
     * \param format		transaction format.  See usrp_spi_defs.h SPI_FMT_*
     * \param buf		the data to write
     * \returns true iff successful
     * Writes are limited to a maximum of 64 bytes.
     *
     * If \p format specifies that optional_header bytes are present, they are
     * written to the peripheral immediately prior to writing \p buf.
     */
    bool write_spi (int optional_header, int enables, int format, const std::string &buf);

    /*
     * \brief Read data from SPI bus peripheral.
     *
     * \param optional_header	0,1 or 2 bytes to write before buf.
     * \param enables		bitmask of peripheral to read. See usrp_spi_defs.h
     * \param format		transaction format.  See usrp_spi_defs.h SPI_FMT_*
     * \param len		number of bytes to read.  Must be in [0,64].
     * \returns the data read if sucessful, else a zero length string.
     *
     * Reads are limited to a maximum of 64 bytes.
     *
     * If \p format specifies that optional_header bytes are present, they
     * are written to the peripheral first.  Then \p len bytes are read from
     * the peripheral and returned.
     */
    std::string read_spi (int optional_header, int enables, int format, int len);
#endif


    class impl;		// implementation details

  private:
    // Static function to retrieve or create usrp2 instance
    static sptr find_existing_or_make_new(const std::string &ifc, props *p);

    // Only class members can instantiate this class
    usrp2(const std::string &ifc, props *p);
  
    // All private state is held in opaque pointer
    std::auto_ptr<impl> d_impl;
  };

};

std::ostream& operator<<(std::ostream &os, const usrp2::props &x);


#endif /* INCLUDED_USRP2_H */
