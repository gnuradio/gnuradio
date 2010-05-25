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

#ifndef INCLUDED_USRP2_H
#define INCLUDED_USRP2_H

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <complex>
#include <usrp2/rx_sample_handler.h>
#include <usrp2/tune_result.h>
#include <usrp2/mimo_config.h>

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

  // FIXME: get from firmware include
  static const int GPIO_TX_BANK = 0;
  static const int GPIO_RX_BANK = 1;

  /*!
   * \brief standard C++ interface to USRP2
   * \ingroup usrp2
   */
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
     * \param rx_bufsize is the length in bytes of the kernel networking buffer to allocate.
     */
    static sptr make(const std::string &ifc, const std::string &addr="", size_t rx_bufsize=0);

    /*!
     * Class destructor
     */
    ~usrp2();

    /*!
     * Returns the MAC address associated with this USRP
     */
    std::string mac_addr();

    /*!
     * Returns the GbE interface name associated with this USRP
     */
    std::string interface_name();

    /*
     * ----------------------------------------------------------------
     * Rx configuration and control
     * ----------------------------------------------------------------
     */

    /*!
     * Set the rx antenna
     */
    bool set_rx_antenna(int ant);

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
     * \brief Set receive daughterboard LO offset frequency
     */
    bool set_rx_lo_offset(double frequency);

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
     * Start streaming receive mode at specified timestamp. USRP2 will send a
     * continuous stream of DSP pipeline samples to host. Call rx_samples(...)
     * to access.
     *
     * \param channel          Stream channel number (0-30)
     * \param items_per_frame  Number of 32-bit items per frame.
     * \param time             Timestamp to start streaming at
     */
    bool start_rx_streaming_at(unsigned int channel=0, unsigned int items_per_frame=0, unsigned int time=0);

    /*!
     * Sync to PPS and start streaming receive mode at specified timestamp.
     * Just like calling sync_to_pps() and start_rx_streaming_at().
     *
     * \param channel          Stream channel number (0-30)
     * \param items_per_frame  Number of 32-bit items per frame.
     * \param time             Timestamp to start streaming at
     */
    bool sync_and_start_rx_streaming_at(unsigned int channel=0, unsigned int items_per_frame=0, uint32_t time=0);

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
     * Set the tx antenna
     */
    bool set_tx_antenna(int ant);

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

    /*
     * \brief Set transmit daughterboard LO offset frequency
     */
    bool set_tx_lo_offset(double frequency);

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

    /*
     * \brief Calculate default scale_iq for given interpolation rate
     */
    void default_tx_scale_iq(int interpolation_factor, int *scale_i, int *scale_q);

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
     * Reset master time to 0 at next PPS edge
     */
    bool sync_to_pps();

    /*!
     * Reset master time to 0 at every PPS edge
     */
    bool sync_every_pps(bool enable);

    /*!
     * Read memory from Wishbone bus as 32-bit words.  Handles endian swapping if needed.
     *
     * \param addr      32-bit aligned address.  Only the lower 16-bits are significant.
     * \param words     Number of 32-bit words
     *
     * \returns         Vector of 32-bit read values
     *
     * WARNING: Attempts to read memory from addresses that do not correspond to RAM or
     * memory-mapped peripherals may cause the USRP2 to hang, requiring a power cycle.
     *
     */
    std::vector<uint32_t> peek32(uint32_t addr, uint32_t words);

    /*!
     * Write memory to Wishbone bus as 32-bit words.  Handles endian swapping if needed.
     *
     * \param addr      32-bit aligned address.  Only the lower 16-bits are significant
     * \param data      Vector of 32-bit values to write.
     *
     * \returns true iff successful
     *
     * WARNING: Attempts to read memory from addresses that do not correspond to RAM or
     * memory-mapped peripherals may cause the USRP2 to hang, requiring a power cycle.
     *
     */
    bool poke32(uint32_t addr, const std::vector<uint32_t> &data);

    /*!
     * Set daughterboard GPIO data direction register.
     *
     * \param bank      GPIO_TX_BANK or GPIO_RX_BANK
     * \param value     16-bits, 0=FPGA input, 1=FPGA output
     * \param mask      16-bits, 0=ignore, 1=set
     *
     * \returns true iff successful
     *
     * WARNING: Improper usage of this function may result in damage to the USRP2
     *
     */
    bool set_gpio_ddr(int bank, uint16_t value, uint16_t mask);

    /*!
     * Set daughterboard GPIO output selection register.  For those GPIO pins that
     * are configured as outputs in the DDR, this settings configures the source
     * of the pin value.
     *
     * \param bank      GPIO_TX_BANK or GPIO_RX_BANK
     * \param sels      Exactly 16 character MSB->LSB string. For each position:
     *                    '.' = ignore this bit, i.e., leave current value
     *                    'a' = Output ATR value
     *                    's' = Output host software controlled value
     *                    '0' = Output FPGA debug bus 0 value
     *                    '1' = Output FPGA debug bus 1 value
     *
     * \returns true iff successful
     *
     * WARNING: Improper usage of this function may result in damage to the USRP2
     *
     */
    bool set_gpio_sels(int bank, std::string sels);

    /*!
     * Set daughterboard GPIO pin values.
     *
     * \param bank     GPIO_TX_BANK or GPIO_RX_BANK
     * \param value    16 bits, 0=low, 1=high
     * \param mask     16 bits, 0=ignore, 1=set
     *
     * \returns true iff successful
     *
     * WARNING: Improper usage of this function may result in damage to the USRP2
     *
     */
    bool write_gpio(int bank, uint16_t value, uint16_t mask);

    /*!
     * Read daughterboard GPIO pin values
     *
     * \param bank     GPIO_TX_BANK or GPIO_RX_BANK
     * \param value    pointer to uint16_t to hold read results
     *
     * \returns true iff successful
     *
     */
    bool read_gpio(int bank, uint16_t *value);

    /*!
     * Set GPIO streaming mode
     *
     * Individually enables streaming GPIO pins through LSBs of DSP
     * samples.
     *
     * On receive, io_rx[15] replaces I[0], io_rx[14] replaces Q[0]
     * On transmit, I[0] maps to io_tx[15], Q[0] maps to io_tx[14]
     * (Transmit streaming is not yet implemented.)
     *
     * The selected GPIO pins must have been set as inputs or outputs
     * and, for transmit, set to software control.
     *
     * When enabled, the replaced DSP sample LSBs become 0.
     *
     * \param bank     GPIO_TX_BANK or GPIO_RX_BANK
     * \param enable   enable[0] controls I channel LSB
     *                 enable[1] controls Q channel LSB
     *
     * \returns true iff successful
     *
     * WARNING: Improper usage of this function may result in damage to the USRP2
     *
     */
    bool enable_gpio_streaming(int bank, int enable);

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
    static sptr find_existing_or_make_new(const std::string &ifc, props *p, size_t rx_bufsize);

    // Only class members can instantiate this class
    usrp2(const std::string &ifc, props *p, size_t rx_bufsize);

    // All private state is held in opaque pointer
    std::auto_ptr<impl> d_impl;
  };

};

std::ostream& operator<<(std::ostream &os, const usrp2::props &x);


#endif /* INCLUDED_USRP2_H */
