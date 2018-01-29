/* -*- c++ -*- */
/* 
 * Copyright 2015,2016 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt_reference_signals_impl.h"
#include <complex>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>

namespace gr {
  namespace dtv {

    //Number of symbols in a frame
    const int dvbt_pilot_gen::d_symbols_per_frame = SYMBOLS_PER_FRAME;
    //Number of frames in a superframe
    const int dvbt_pilot_gen::d_frames_per_superframe = FRAMES_PER_SUPERFRAME;

    // 2k mode
    // Scattered pilots # of carriers
    const int dvbt_pilot_gen::d_spilot_carriers_size_2k = SCATTERED_PILOT_SIZE_2k;
    // Continual pilots # of carriers and positions
    const int dvbt_pilot_gen::d_cpilot_carriers_size_2k = CONTINUAL_PILOT_SIZE_2k;
    const int dvbt_pilot_gen::d_cpilot_carriers_2k[dvbt_pilot_gen::d_cpilot_carriers_size_2k] = {
       0,   48,   54,   87,  141,  156,  192, \
     201,  255,  279,  282,  333,  432,  450, \
     483,  525,  531,  618,  636,  714,  759, \
     765,  780,  804,  873,  888,  918,  939, \
     942,  969,  984, 1050, 1101, 1107, 1110, \
    1137, 1140, 1146, 1206, 1269, 1323, 1377, \
    1491, 1683, 1704
    };
    // TPS pilots # of carriers and positions
    const int dvbt_pilot_gen::d_tps_carriers_size_2k = TPS_PILOT_SIZE_2k;
    const int dvbt_pilot_gen::d_tps_carriers_2k[dvbt_pilot_gen::d_tps_carriers_size_2k] = {
      34,   50,  209,  346,  413, \
     569,  595,  688,  790,  901, \
    1073, 1219, 1262, 1286, 1469, \
    1594, 1687
    };

    // 8k mode
    // Scattered pilots # of carriers
    const int dvbt_pilot_gen::d_spilot_carriers_size_8k = SCATTERED_PILOT_SIZE_8k;
    // Continual pilots # of carriers and positions
    const int dvbt_pilot_gen::d_cpilot_carriers_size_8k = CONTINUAL_PILOT_SIZE_8k;
    const int dvbt_pilot_gen::d_cpilot_carriers_8k[dvbt_pilot_gen::d_cpilot_carriers_size_8k] = {
       0,   48,   54,   87,  141,  156,  192,
     201,  255,  279,  282,  333,  432,  450,
     483,  525,  531,  618,  636,  714,  759,
     765,  780,  804,  873,  888,  918,  939,
     942,  969,  984, 1050, 1101, 1107, 1110,
    1137, 1140, 1146, 1206, 1269, 1323, 1377,
    1491, 1683, 1704, 1752, 1758, 1791, 1845,
    1860, 1896, 1905, 1959, 1983, 1986, 2037,
    2136, 2154, 2187, 2229, 2235, 2322, 2340,
    2418, 2463, 2469, 2484, 2508, 2577, 2592,
    2622, 2643, 2646, 2673, 2688, 2754, 2805,
    2811, 2814, 2841, 2844, 2850, 2910, 2973,
    3027, 3081, 3195, 3387, 3408, 3456, 3462,
    3495, 3549, 3564, 3600, 3609, 3663, 3687,
    3690, 3741, 3840, 3858, 3891, 3933, 3939,
    4026, 4044, 4122, 4167, 4173, 4188, 4212,
    4281, 4296, 4326, 4347, 4350, 4377, 4392,
    4458, 4509, 4515, 4518, 4545, 4548, 4554,
    4614, 4677, 4731, 4785, 4899, 5091, 5112,
    5160, 5166, 5199, 5253, 5268, 5304, 5313,
    5367, 5391, 5394, 5445, 5544, 5562, 5595,
    5637, 5643, 5730, 5748, 5826, 5871, 5877,
    5892, 5916, 5985, 6000, 6030, 6051, 6054,
    6081, 6096, 6162, 6213, 6219, 6222, 6249,
    6252, 6258, 6318, 6381, 6435, 6489, 6603,
    6795, 6816
    };
    // TPS pilots # of carriers and positions
    const int dvbt_pilot_gen::d_tps_carriers_size_8k = TPS_PILOT_SIZE_8k;
    const int dvbt_pilot_gen::d_tps_carriers_8k[dvbt_pilot_gen::d_tps_carriers_size_8k] = {
      34,   50,  209,  346,  413,  569,  595,  688, \
     790,  901, 1073, 1219, 1262, 1286, 1469, 1594, \
    1687, 1738, 1754, 1913, 2050, 2117, 2273, 2299, \
    2392, 2494, 2605, 2777, 2923, 2966, 2990, 3173, \
    3298, 3391, 3442, 3458, 3617, 3754, 3821, 3977, \
    4003, 4096, 4198, 4309, 4481, 4627, 4670, 4694, \
    4877, 5002, 5095, 5146, 5162, 5321, 5458, 5525, \
    5681, 5707, 5800, 5902, 6013, 6185, 6331, 6374, \
    6398, 6581, 6706, 6799
    };

    // TPS sync sequence for odd and even frames
    const int dvbt_pilot_gen::d_tps_sync_size = 16; // TODO
    const int dvbt_pilot_gen::d_tps_sync_even[d_tps_sync_size] = {
      0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0
    };
    const int dvbt_pilot_gen::d_tps_sync_odd[d_tps_sync_size] = {
      1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1
    };

    /*
     * Constructor of class
     */
    dvbt_pilot_gen::dvbt_pilot_gen(const dvbt_configure &c) : config(c),
          d_spilot_index(0),
          d_cpilot_index(0),
          d_tpilot_index(0),
          d_symbol_index(0),
          d_symbol_index_known(0),
          d_frame_index(0),
          d_superframe_index(0),
          d_freq_offset_max(8),
          d_trigger_index(0),
          d_payload_index(0),
          d_chanestim_index(0),
          d_prev_mod_symbol_index(0),
          d_mod_symbol_index(0)
    {
      //Determine parameters from config file
      d_Kmin = config.d_Kmin;
      d_Kmax = config.d_Kmax;
      d_fft_length = config.d_fft_length;
      d_payload_length = config.d_payload_length;
      d_zeros_on_left = config.d_zeros_on_left;
      d_zeros_on_right = config.d_zeros_on_right;
      d_cp_length = config.d_cp_length;

      //Set-up pilot data depending on transmission mode
      if (config.d_transmission_mode == T2k) {
        d_spilot_carriers_size = d_spilot_carriers_size_2k;
        d_cpilot_carriers_size = d_cpilot_carriers_size_2k;
        d_cpilot_carriers = d_cpilot_carriers_2k;
        d_tps_carriers_size = d_tps_carriers_size_2k;
        d_tps_carriers = d_tps_carriers_2k;
      }
      else if (config.d_transmission_mode == T8k) {
        d_spilot_carriers_size = d_spilot_carriers_size_8k;
        d_cpilot_carriers_size = d_cpilot_carriers_size_8k;
        d_cpilot_carriers = d_cpilot_carriers_8k;
        d_tps_carriers_size = d_tps_carriers_size_8k;
        d_tps_carriers = d_tps_carriers_8k;
      }
      else {
        d_spilot_carriers_size = d_spilot_carriers_size_2k;
        d_cpilot_carriers_size = d_cpilot_carriers_size_2k;
        d_cpilot_carriers = d_cpilot_carriers_2k;
        d_tps_carriers_size = d_tps_carriers_size_2k;
        d_tps_carriers = d_tps_carriers_2k;
      }

      d_freq_offset = 0;
      d_carrier_freq_correction = 0.0;
      d_sampling_freq_correction = 0.0;

      //allocate PRBS buffer
      d_wk = new char[d_Kmax - d_Kmin + 1];
      if (d_wk == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_wk." << std::endl;
        throw std::bad_alloc();
      }
      // Generate wk sequence
      generate_prbs();

      // allocate buffer for scattered pilots
      d_spilot_carriers_val = new (std::nothrow) gr_complex[d_Kmax - d_Kmin + 1];
      if (d_spilot_carriers_val == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_spilot_carriers_val." << std::endl;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // allocate buffer for channel gains (for each useful carrier)
      d_channel_gain = new (std::nothrow) gr_complex[d_Kmax - d_Kmin + 1];
      if (d_channel_gain == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_channel_gain." << std::endl;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // Allocate buffer for continual pilots phase diffs
      d_known_phase_diff = new (std::nothrow) float[d_cpilot_carriers_size - 1];
      if (d_known_phase_diff == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_known_phase_diff." << std::endl;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // Obtain phase diff for all continual pilots
      for (int i = 0; i < (d_cpilot_carriers_size - 1); i++) {
        d_known_phase_diff[i] = \
          norm(get_cpilot_value(d_cpilot_carriers[i + 1]) - get_cpilot_value(d_cpilot_carriers[i]));
      }

      d_cpilot_phase_diff = new (std::nothrow) float[d_cpilot_carriers_size - 1];
      if (d_cpilot_phase_diff == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_cpilot_phase_diff." << std::endl;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // Allocate buffer for derotated input symbol
      d_derot_in = new (std::nothrow) gr_complex[d_fft_length];
      if (d_derot_in == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_derot_in." << std::endl;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // allocate buffer for first tps symbol constellation
      d_tps_carriers_val = new (std::nothrow) gr_complex[d_tps_carriers_size];
      if (d_tps_carriers_val == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_tps_carriers_val." << std::endl;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // allocate tps data buffer
      d_tps_data = new (std::nothrow) unsigned char[d_symbols_per_frame];
      if (d_tps_data == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_tps_data." << std::endl;
        delete [] d_tps_carriers_val;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      d_prev_tps_symbol = new (std::nothrow) gr_complex[d_tps_carriers_size];
      if (d_prev_tps_symbol == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_prev_tps_symbol." << std::endl;
        delete [] d_tps_data;
        delete [] d_tps_carriers_val;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }
      memset(d_prev_tps_symbol, 0, d_tps_carriers_size * sizeof(gr_complex));

      d_tps_symbol = new (std::nothrow) gr_complex[d_tps_carriers_size];
      if (d_tps_symbol == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_tps_symbol." << std::endl;
        delete [] d_prev_tps_symbol;
        delete [] d_tps_data;
        delete [] d_tps_carriers_val;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }
      memset(d_tps_symbol, 0, d_tps_carriers_size * sizeof(gr_complex));

      // Init receive TPS data vector
      for (int i = 0; i < d_symbols_per_frame; i++) {
        d_rcv_tps_data.push_back(0);
      }

      // Init TPS sync sequence
      for (int i = 0; i < d_tps_sync_size; i++) {
        d_tps_sync_evenv.push_back(d_tps_sync_even[i]);
        d_tps_sync_oddv.push_back(d_tps_sync_odd[i]);
      }

      // Allocate buffer for channel estimation carriers
      d_chanestim_carriers = new (std::nothrow) int[d_Kmax - d_Kmin + 1];
      if (d_chanestim_carriers == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_chanestim_carriers." << std::endl;
        delete [] d_tps_symbol;
        delete [] d_prev_tps_symbol;
        delete [] d_tps_data;
        delete [] d_tps_carriers_val;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // Allocate buffer for payload carriers
      d_payload_carriers = new (std::nothrow) int[d_Kmax - d_Kmin + 1];
      if (d_payload_carriers == NULL) {
        std::cerr << "Reference Signals, cannot allocate memory for d_payload_carriers." << std::endl;
        delete [] d_chanestim_carriers;
        delete [] d_tps_symbol;
        delete [] d_prev_tps_symbol;
        delete [] d_tps_data;
        delete [] d_tps_carriers_val;
        delete [] d_derot_in;
        delete [] d_cpilot_phase_diff;
        delete [] d_known_phase_diff;
        delete [] d_channel_gain;
        delete [] d_spilot_carriers_val;
        delete [] d_wk;
        throw std::bad_alloc();
      }

      // Reset the pilot generator
      reset_pilot_generator();
      // Format TPS data with current values
      format_tps_data();
    }

    /*
     * Destructor of class
     */
    dvbt_pilot_gen::~dvbt_pilot_gen()
    {
      delete [] d_payload_carriers;
      delete [] d_chanestim_carriers;
      delete [] d_tps_symbol;
      delete [] d_prev_tps_symbol;
      delete [] d_tps_data;
      delete [] d_tps_carriers_val;
      delete [] d_derot_in;
      delete [] d_cpilot_phase_diff;
      delete [] d_known_phase_diff;
      delete [] d_channel_gain;
      delete [] d_spilot_carriers_val;
      delete [] d_wk;
    }

    /*
     * Generate PRBS sequence
     * X^11 + X^2 + 1
     * en 300 744 - section 4.5.2
     */
    void
    dvbt_pilot_gen::generate_prbs()
    {
      // init PRBS register with 1s
      unsigned int reg_prbs = (1 << 11) - 1;

      for (int k = 0; k < (d_Kmax - d_Kmin + 1); k++) {
        d_wk[k] = (char)(reg_prbs & 0x01);
        int new_bit = ((reg_prbs >> 2) ^ (reg_prbs >> 0)) & 0x01;
        reg_prbs = (reg_prbs >> 1) | (new_bit << 10);
      }
    }

    /*
     * Generate shortened BCH(67, 53) codes from TPS data
     * Extend the code with 60 bits and use BCH(127, 113)
     */
    void
    dvbt_pilot_gen::generate_bch_code()
    {
      //TODO
      //DO other way: if (feedback == 1) reg = reg ^ polymomial
      //else nothing

      //(n, k) = (127, 113) = (60+67, 60+53)
      unsigned int reg_bch = 0;
      unsigned char data_in[113];

      //fill in 60 zeros
      memset(&data_in[0], 0, 60);
      //fill in TPS data - start bit not included
      memcpy(&data_in[60], &d_tps_data[1], 53);

      //X^14+X^9+X^8+X^6+X^5+X^4+X^2+X+1
      for (int i = 0; i < 113; i++) {
        int feedback = 0x1 & (data_in[i] ^ reg_bch);
        reg_bch  = reg_bch >> 1;
        reg_bch |= feedback << 13;
        reg_bch = reg_bch \
            ^ (feedback << 12) ^ (feedback << 11) \
            ^ (feedback << 9) ^ (feedback << 8) \
            ^ (feedback << 7) ^ (feedback << 5) \
            ^ (feedback << 4);
      }

      for (int i = 0; i < 14; i++) {
        d_tps_data[i + 54] = 0x1 & (reg_bch >> i);
      }
    }

    int
    dvbt_pilot_gen::verify_bch_code(std::deque<char> data)
    {
      int ret = 0;

      //TODO
      //DO other way: if (feedback == 1) reg = reg ^ polymomial
      //else nothing

      //(n, k) = (127, 113) = (60+67, 60+53)
      unsigned int reg_bch = 0;
      unsigned char data_in[113];

      //fill in 60 zeros
      memset(&data_in[0], 0, 60);
      //fill in TPS data - start bit not included
      //memcpy(&data_in[60], &data[1], 53);
      for (int i = 0; i < 53; i++) {
        data_in[60 + i] = data[1 + i];
      }

      //X^14+X^9+X^8+X^6+X^5+X^4+X^2+X+1
      for (int i = 0; i < 113; i++) {
        int feedback = 0x1 & (data_in[i] ^ reg_bch);
        reg_bch  = reg_bch >> 1;
        reg_bch |= feedback << 13;
        reg_bch = reg_bch \
            ^ (feedback << 12) ^ (feedback << 11) \
            ^ (feedback << 9) ^ (feedback << 8) \
            ^ (feedback << 7) ^ (feedback << 5) \
            ^ (feedback << 4);
      }

      for (int i = 0; i < 14; i++) {
        if ((unsigned int)data[i + 54] != (0x1 & (reg_bch >> i))) {
          ret = -1;
          break;
        }
      }

      return ret;
    }

    void
    dvbt_pilot_gen::set_symbol_index(int sindex)
    {
      d_symbol_index = sindex;
    }

    int
    dvbt_pilot_gen::get_symbol_index()
    {
      return d_symbol_index;
    }

    void
    dvbt_pilot_gen::set_tps_data()
    {
    }

    void
    dvbt_pilot_gen::get_tps_data()
    {
    }

    /*
     * Reset pilot generator
     */
    void
    dvbt_pilot_gen::reset_pilot_generator()
    {
      d_spilot_index = 0; d_cpilot_index = 0; d_tpilot_index = 0;
      d_payload_index = 0; d_chanestim_index = 0;
      d_symbol_index = 0; d_frame_index = 0; d_superframe_index = 0;
      d_symbol_index_known = 0;
      d_equalizer_ready = 0;
    }

    /*
     * Init scattered pilot generator
     */
    int
    dvbt_pilot_gen::get_current_spilot(int sindex) const
    {
      //TODO - can be optimized for same symbol_index
      return (d_Kmin + 3 * (sindex % 4) + 12 * d_spilot_index);
    }

    gr_complex
    dvbt_pilot_gen::get_spilot_value(int spilot)
    {
      // TODO - can be calculated at the beginning
      return gr_complex(4 * 2 * (0.5 - d_wk[spilot]) / 3, 0);
    }

    void
    dvbt_pilot_gen::set_spilot_value(int spilot, gr_complex val)
    {
      d_spilot_carriers_val[spilot] = val;
    }

    void
    dvbt_pilot_gen::set_channel_gain(int spilot, gr_complex val)
    {
      // Gain gval=rxval/txval
      d_channel_gain[spilot] = gr_complex((4 * 2 * (0.5 - d_wk[spilot]) / 3), 0) / val;
    }
    void
    dvbt_pilot_gen::advance_spilot(int sindex)
    {
      //TODO - do in a simpler way?
      int size = d_spilot_carriers_size;

      if (sindex == 0) {
        size = d_spilot_carriers_size + 1;
      }

      // TODO - fix this - what value should we use?
      ++d_spilot_index;
      d_spilot_index = d_spilot_index % size;
    }

    int
    dvbt_pilot_gen::get_first_spilot()
    {
      d_spilot_index = 0;

      return (d_Kmin + 3 * (d_symbol_index % 4));
    }

    int
    dvbt_pilot_gen::get_last_spilot() const
    {
      int size = d_spilot_carriers_size - 1;

      if (d_symbol_index == 0) {
        size = d_spilot_carriers_size;
      }

      return (d_Kmin + 3 * (d_symbol_index % 4) + 12 * size);
    }

    int
    dvbt_pilot_gen::get_next_spilot()
    {
      int pilot = (d_Kmin + 3 * (d_symbol_index % 4) + 12 * (++d_spilot_index));

      if (pilot > d_Kmax) {
        pilot = d_Kmax;
      }

      return pilot;
    }

    int
    dvbt_pilot_gen::process_spilot_data(const gr_complex * in)
    {
      // This is channel estimator
      // Interpolate the gain between carriers to obtain
      // gain for non pilot carriers - we use linear interpolation

      /*************************************************************/
      // Find out the OFDM symbol index (value 0 to 3) sent
      // in current block by correlating scattered symbols with
      // current block - result is (symbol index % 4)
      /*************************************************************/
      float max = 0; float sum = 0;

      for (int scount = 0; scount < 4; scount++) {
        d_spilot_index = 0; d_cpilot_index = 0;
        d_chanestim_index = 0;

        for (int k = 0; k < (d_Kmax - d_Kmin + 1); k++) {
          // Keep data for channel estimation
          if (k == get_current_spilot(scount)) {
            set_chanestim_carrier(k);
            advance_spilot(scount);
            advance_chanestim();
          }
        }

        gr_complex c = gr_complex(0.0, 0.0);

        // This should be of range 0 to d_chanestim_index bit for now we use just a
        // small number of spilots to obtain the symbol index
        for (int j = 0; j < 10; j++) {
          c += get_spilot_value(d_chanestim_carriers[j]) * conj(in[d_zeros_on_left + d_chanestim_carriers[j]]);
        }
        sum = norm(c);

        if (sum > max) {
          max = sum;
          d_mod_symbol_index = scount;
        }
      }

      /*************************************************************/
      // Keep data for channel estimator
      // This method interpolates scattered measurements across one OFDM symbol
      // It does not use measurements from the previous OFDM symbols (does not use history)
      // as it may have encountered a phase change for the current phase only
      /*************************************************************/

      d_spilot_index = 0; d_cpilot_index = 0;
      d_chanestim_index = 0;

      for (int k = 0; k < (d_Kmax - d_Kmin + 1); k++) {
        // Keep data for channel estimation
        if (k == get_current_spilot(d_mod_symbol_index)) {
          set_chanestim_carrier(k);
          advance_spilot(d_mod_symbol_index);
          advance_chanestim();
        }

        // Keep data for channel estimation
        if (k == get_current_cpilot()) {
          set_chanestim_carrier(k);
          advance_cpilot();
          advance_chanestim();
        }
      }

      // We use both scattered pilots and continual pilots
      for (int i = 0, startk = d_chanestim_carriers[0]; i < d_chanestim_index; i++) {
        // Get a carrier from the list of carriers
        // used for channel estimation
        int k = d_chanestim_carriers[i];

        set_channel_gain(k, in[k + d_zeros_on_left]);

        // Calculate tg(alpha) due to linear interpolation
        gr_complex tg_alpha = (d_channel_gain[k] - d_channel_gain[startk]) / gr_complex(11.0, 0.0);

        // Calculate interpolation for all intermediate values
        for (int j = 1; j < (k - startk); j++) {
          gr_complex current = d_channel_gain[startk] + tg_alpha * gr_complex(j, 0.0);
          d_channel_gain[startk + j] = current;
        }

        startk = k;
      }

      // Signal that equalizer is ready
      d_equalizer_ready = 1;

      int diff_sindex = (d_mod_symbol_index - d_prev_mod_symbol_index + 4) % 4;

      d_prev_mod_symbol_index = d_mod_symbol_index;

      return diff_sindex;
    }

    /*
     * Init continual pilot generator
     */
    int
    dvbt_pilot_gen::get_current_cpilot() const
    {
      return d_cpilot_carriers[d_cpilot_index];
    }

    gr_complex
    dvbt_pilot_gen::get_cpilot_value(int cpilot)
    {
      //TODO - can be calculated at the beginning
      return gr_complex((float)(4 * 2 * (0.5 - d_wk[cpilot])) / 3, 0);
    }

    void
    dvbt_pilot_gen::advance_cpilot()
    {
      ++d_cpilot_index;
      d_cpilot_index = d_cpilot_index % d_cpilot_carriers_size;
    }

    void
    dvbt_pilot_gen::process_cpilot_data(const gr_complex * in)
    {
      // Look for maximum correlation for cpilots
      // in order to obtain post FFT integer frequency correction

      float max = 0; float sum = 0;
      int start = 0;
      float phase;

      for (int i = d_zeros_on_left - d_freq_offset_max; i < d_zeros_on_left + d_freq_offset_max; i++) {
        sum = 0;
        for (int j = 0; j < (d_cpilot_carriers_size - 1); j++) {
          phase = norm(in[i + d_cpilot_carriers[j + 1]] - in[i + d_cpilot_carriers[j]]);
          sum += d_known_phase_diff[j] * phase;
        }

        if (sum > max) {
          max = sum;
          start = i;
        }
      }

      d_freq_offset = start - d_zeros_on_left;
    }

    void
    dvbt_pilot_gen::compute_oneshot_csft(const gr_complex * in)
    {
      gr_complex left_corr_sum = 0.0; gr_complex right_corr_sum = 0.0;
      int half_size = (d_cpilot_carriers_size - 1) / 2;

      // TODO init this in constructor
      float carrier_coeff = 1.0 / (2 * M_PI * (1 + float (d_cp_length) / float (d_fft_length)) * 2);
      float sampling_coeff = 1.0 / (2 * M_PI * ((1 + float (d_cp_length) / float (d_fft_length)) * ((float)d_cpilot_carriers_size / 2.0)));

      float left_angle, right_angle;

      // Compute cpilots correlation between previous symbol and current symbol
      // in both halves of the cpilots. The cpilots are distributed evenly
      // on left and right sides of the center frequency.

      for (int j = 0; j < half_size; j++) {
        left_corr_sum += in[d_freq_offset + d_zeros_on_left + d_cpilot_carriers[j]] * \
                         std::conj(in[d_freq_offset + d_fft_length + d_zeros_on_left + d_cpilot_carriers[j]]);
      }

      for (int j = half_size + 1; j < d_cpilot_carriers_size; j++) {
        right_corr_sum += in[d_freq_offset + d_zeros_on_left + d_cpilot_carriers[j]] * \
                          std::conj(in[d_freq_offset + d_fft_length + d_zeros_on_left + d_cpilot_carriers[j]]);
      }

      left_angle = std::arg(left_corr_sum);
      right_angle = std::arg(right_corr_sum);

      d_carrier_freq_correction = (right_angle + left_angle) * carrier_coeff;
      d_sampling_freq_correction = (right_angle - left_angle) * sampling_coeff;
    }

    gr_complex *
    dvbt_pilot_gen::frequency_correction(const gr_complex * in, gr_complex * out)
    {
      // TODO - use PI control loop to calculate tracking corrections
      int symbol_count = 1;

      for (int k = 0; k < d_fft_length; k++) {
        // TODO - for 2k mode the continuous pilots are not split evenly
        // between left/right center frequency. Probably the scattered
        // pilots needs to be added.

        float correction = (float)d_freq_offset + d_carrier_freq_correction;

        gr_complex c = gr_expj(-2 * M_PI * correction * \
          (d_fft_length + d_cp_length) / d_fft_length * symbol_count);

        // TODO - vectorize this operation
        out[k] = c * in[k + d_freq_offset];
      }

      return (out);
    }

    /*
     * Init tps sequence, return values for first position
     * If first symbol then init tps DBPSK data
     */
    int
    dvbt_pilot_gen::get_current_tpilot() const
    {
      return d_tps_carriers[d_tpilot_index];
    }

    gr_complex
    dvbt_pilot_gen::get_tpilot_value(int tpilot)
    {
      //TODO - it can be calculated at the beginnning
      if (d_symbol_index == 0) {
        d_tps_carriers_val[d_tpilot_index] = gr_complex(2 * (0.5 - d_wk[tpilot]), 0);
      }
      else {
        if (d_tps_data[d_symbol_index] == 1) {
          d_tps_carriers_val[d_tpilot_index] = gr_complex(-d_tps_carriers_val[d_tpilot_index].real(), 0);
        }
      }

      return d_tps_carriers_val[d_tpilot_index];
    }

    void
    dvbt_pilot_gen::advance_tpilot()
    {
      ++d_tpilot_index;
      d_tpilot_index = d_tpilot_index % d_tps_carriers_size;
    }

    /*
     * Set a number of bits to a specified value
     */
    void
    dvbt_pilot_gen::set_tps_bits(int start, int stop, unsigned int data)
    {
      for (int i = start; i >= stop; i--) {
        d_tps_data[i] = data & 0x1;
        data = data >> 1;
      }
    }

    /*
     * Clause 4.6
     * Format data that will be sent with TPS signals
     * en 300 744 - section 4.6.2
     * s0 Initialization
     * s1-s16 Synchronization word
     * s17-s22 Length Indicator
     * s23-s24 Frame Number
     * S25-s26 Constellation
     * s27, s28, s29 Hierarchy information
     * s30, s31, s32 Code rate, HP stream
     * s33, s34, s35 Code rate, LP stream
     * s36, s37 Guard interval
     * s38, s39 Transmission mode
     * s40, s47 Cell identifier
     * s48-s53 All set to "0"
     * s54-s67 Error protection (BCH code)
     */
    void
    dvbt_pilot_gen::format_tps_data()
    {
      //Clause 4.6.3
      set_tps_bits(0, 0, d_wk[0]);
      //Clause 4.6.2.2
      if (d_frame_index % 2) {
        set_tps_bits(16, 1, 0xca11);
      }
      else {
        set_tps_bits(16, 1, 0x35ee);
      }
      //Clause 4.6.2.3
      if (config.d_include_cell_id) {
        set_tps_bits(22, 17, 0x1f);
      }
      else {
        set_tps_bits(22, 17, 0x17);
      }
      //Clause 4.6.2.4
      set_tps_bits(24, 23, d_frame_index);
      //Clause 4.6.2.5
      set_tps_bits(26, 25, config.d_constellation);
      //Clause 4.6.2.6
      set_tps_bits(29, 27, config.d_hierarchy);
      //Clause 4.6.2.7
      switch (config.d_code_rate_HP) {
        case C1_2:
          set_tps_bits(32, 30, 0);
          break;
        case C2_3:
          set_tps_bits(32, 30, 1);
          break;
        case C3_4:
          set_tps_bits(32, 30, 2);
          break;
        case C5_6:
          set_tps_bits(32, 30, 3);
          break;
        case C7_8:
          set_tps_bits(32, 30, 4);
          break;
        default:
          set_tps_bits(32, 30, 0);
          break;
      }
      switch (config.d_code_rate_LP) {
        case C1_2:
          set_tps_bits(35, 33, 0);
          break;
        case C2_3:
          set_tps_bits(35, 33, 1);
          break;
        case C3_4:
          set_tps_bits(35, 33, 2);
          break;
        case C5_6:
          set_tps_bits(35, 33, 3);
          break;
        case C7_8:
          set_tps_bits(35, 33, 4);
          break;
        default:
          set_tps_bits(35, 33, 0);
          break;
      }
      //Clause 4.6.2.8
      set_tps_bits(37, 36, config.d_guard_interval);
      //Clause 4.6.2.9
      set_tps_bits(39, 38, config.d_transmission_mode);
      //Clause 4.6.2.10
      if (d_frame_index % 2) {
        set_tps_bits(47, 40, config.d_cell_id & 0xff);
      }
      else {
        set_tps_bits(47, 40, (config.d_cell_id >> 8) & 0xff);
      }
      //These bits are set to zero
      set_tps_bits(53, 48, 0);
      //Clause 4.6.2.11
      generate_bch_code();
    }

    int
    dvbt_pilot_gen::process_tps_data(const gr_complex * in, const int diff_symbol_index)
    {
      int end_frame = 0;

      // Look for TPS data only - demodulate DBPSK
      // Calculate phase difference between previous symbol
      // and current one to determine the current bit.
      // Use majority voting for decision
      int tps_majority_zero = 0;

      for (int k = 0; k < d_tps_carriers_size; k++) {
        // Use equalizer to correct data and frequency correction
        gr_complex val = in[d_zeros_on_left + d_tps_carriers[k]] * d_channel_gain[d_tps_carriers[k]];

        if (!d_symbol_index_known || (d_symbol_index != 0)) {
          gr_complex phdiff = val * conj(d_prev_tps_symbol[k]);

          if (phdiff.real() >= 0.0) {
            tps_majority_zero++;
          }
          else {
            tps_majority_zero--;
          }
        }

        d_prev_tps_symbol[k] = val;
      }

      // Insert obtained TPS bit into FIFO
      // Insert the same bit into FIFO in the case
      // diff_symbol_index is more than one. This will happen
      // in the case of losing 1 to 3 symbols.
      // This could be corrected by BCH decoder afterwards.

      for (int i = 0; i < diff_symbol_index; i++) {
        // Take out the front entry first
        d_rcv_tps_data.pop_front();

        // Add data at tail
        if (!d_symbol_index_known || (d_symbol_index != 0)) {
          if (tps_majority_zero >= 0) {
            d_rcv_tps_data.push_back(0);
          }
          else {
            d_rcv_tps_data.push_back(1);
          }
        }
        else {
          d_rcv_tps_data.push_back(0);
        }
      }

      // Match synchronization signatures
      if (std::equal(d_rcv_tps_data.begin() + 1, d_rcv_tps_data.begin() + d_tps_sync_evenv.size(), d_tps_sync_evenv.begin())) {
        // Verify parity for TPS data
        if (!verify_bch_code(d_rcv_tps_data)) {
          d_frame_index = (d_rcv_tps_data[23] << 1) | (d_rcv_tps_data[24]);

          d_symbol_index_known = 1;
          end_frame = 1;
        }
        else {
          d_symbol_index_known = 0;
          end_frame = 0;
        }

        // Clear up FIFO
        for (int i = 0; i < d_symbols_per_frame; i++) {
          d_rcv_tps_data[i] = 0;
        }
      }
      else if (std::equal(d_rcv_tps_data.begin() + 1, d_rcv_tps_data.begin() + d_tps_sync_oddv.size(), d_tps_sync_oddv.begin())) {
        // Verify parity for TPS data
        if (!verify_bch_code(d_rcv_tps_data)) {
          d_frame_index = (d_rcv_tps_data[23] << 1) | (d_rcv_tps_data[24]);

          d_symbol_index_known = 1;
          end_frame = 1;
        }
        else {
          d_symbol_index_known = 0;
          end_frame = 0;
        }

        // Clear up FIFO
        for (int i = 0; i < d_symbols_per_frame; i++) {
          d_rcv_tps_data[i] = 0;
        }
      }

      return end_frame;
    }

    void
    dvbt_pilot_gen::set_chanestim_carrier(int k)
    {
      d_chanestim_carriers[d_chanestim_index] = k;
    }

    void
    dvbt_pilot_gen::advance_chanestim()
    {
      d_chanestim_index++;
    }

    int
    dvbt_pilot_gen::get_current_payload()
    {
      return d_payload_carriers[d_payload_index];
    }

    void
    dvbt_pilot_gen::set_payload_carrier(int k)
    {
      d_payload_carriers[d_payload_index] = k;
    }

    void
    dvbt_pilot_gen::advance_payload()
    {
      d_payload_index++;
    }

    void
    dvbt_pilot_gen::process_payload_data(const gr_complex *in, gr_complex *out)
    {
      //reset indexes
      d_spilot_index = 0; d_cpilot_index = 0; d_tpilot_index = 0;
      d_payload_index = 0;d_chanestim_index = 0;
      int is_payload = 1;

      //process one block - one symbol
      for (int k = 0; k < (d_Kmax - d_Kmin + 1); k++) {
        is_payload = 1;

        // Keep data for channel estimation
        // This depends on the symbol index
        if (k == get_current_spilot(d_mod_symbol_index)) {
          advance_spilot(d_mod_symbol_index);
          is_payload = 0;
        }

        // Keep data for frequency correction
        // and channel estimation
        if (k == get_current_cpilot()) {
          advance_cpilot();
          is_payload = 0;
        }

        if (k == get_current_tpilot()) {
          advance_tpilot();
          is_payload = 0;
        }

        // Keep payload carrier number
        // This depends on the symbol index
        if (is_payload) {
          set_payload_carrier(k);
          advance_payload();
        }
      }

      if (d_equalizer_ready) {
        // Equalize payload data according to channel estimator
        for (int i = 0; i < d_payload_index; i++) {
          out[i] = in[d_zeros_on_left + d_payload_carriers[i]] * d_channel_gain[d_payload_carriers[i]];
        }
      }
      else {
        // If equ not ready, return 0
        for (int i = 0; i < d_payload_length; i++) {
          out[0] = gr_complex(0.0, 0.0);
        }
      }
    }

    void
    dvbt_pilot_gen::update_output(const gr_complex *in, gr_complex *out)
    {
      int is_payload = 1;
      int payload_count = 0;

      //move to the next symbol
      //re-genereate TPS data
      format_tps_data();

      //reset indexes
      payload_count = 0;
      d_spilot_index = 0; d_cpilot_index = 0; d_tpilot_index = 0;

      for (int i = 0; i < d_zeros_on_left; i++) {
        out[i] = gr_complex(0.0, 0.0);
      }

      //process one block - one symbol
      for (int k = d_Kmin; k < (d_Kmax - d_Kmin + 1); k++) {
          is_payload = 1;
          if (k == get_current_spilot(d_symbol_index)) {
            out[d_zeros_on_left + k] = get_spilot_value(k);
            advance_spilot(d_symbol_index);
            is_payload = 0;
          }

          if (k == get_current_cpilot()) {
            out[d_zeros_on_left + k] = get_cpilot_value(k);
            advance_cpilot();
            is_payload = 0;
          }

          if (k == get_current_tpilot()) {
            out[d_zeros_on_left + k] = get_tpilot_value(k);
            advance_tpilot();
            is_payload = 0;
          }

          if (is_payload == 1) {
            out[d_zeros_on_left + k] = in[payload_count++];
          }
      }

      // update indexes
      if (++d_symbol_index == d_symbols_per_frame) {
        d_symbol_index = 0;
        if (++d_frame_index == d_frames_per_superframe) {
          d_frame_index = 0;
          d_superframe_index++;
        }
      }

      for (int i = (d_fft_length - d_zeros_on_right); i < d_fft_length; i++) {
        out[i] = gr_complex(0.0, 0.0);
      }
    }

    int
    dvbt_pilot_gen::parse_input(const gr_complex *in, gr_complex *out, int * symbol_index, int * frame_index)
    {
      d_trigger_index++;

      // Obtain frequency correction based on cpilots.
      // Obtain channel estimation based on both
      // cpilots and spilots.
      // We use spilot correlation for finding the symbol index modulo 4
      // The diff between previous sym index and current index is used
      // to advance the symbol index inside a frame (0 to 67)
      // Then based on the TPS data we find out the start of a frame

      // Process cpilot data
      // This is post FFT integer frequency offset estimation
      // This is called before all other processing
      process_cpilot_data(in);

      // Compute one shot Post-FFT Carrier and Sampling Frequency Tracking
      // Obtain fractional Carrer and Sampling frequency corrections
      // Before this moment it is assumed to have corrected this:
      // - symbol timing (pre-FFT)
      // - symbol frequency correction (pre-FFT)
      // - integer frequency correction (post-FFT)
      // TODO - call this just in the acquisition mode
      compute_oneshot_csft(in);

      // Gather all corrections and obtain a corrected OFDM symbol:
      // - input symbol shift (post-FFT)
      // - integer frequency correction (post-FFT)
      // - fractional frequency (carrier and sampling) corrections (post-FFT)
      // TODO - use PI to update the corrections
      frequency_correction(in, d_derot_in);

      // Process spilot data
      // This is channel estimation function
      int diff_symbol_index = process_spilot_data(d_derot_in);

      // Correct symbol index so that all subsequent processing
      // use correct symbol index
      d_symbol_index = (d_symbol_index + diff_symbol_index) % d_symbols_per_frame;

      // Symbol index is used in other modules too
      *symbol_index = d_symbol_index;
      // Frame index is used in other modules too
      *frame_index = d_frame_index;

      // Process TPS data
      // If a frame is recognized then signal end of frame
      int frame_end = process_tps_data(d_derot_in, diff_symbol_index);

      // We are just at the end of a frame
      if (frame_end) {
        d_symbol_index = d_symbols_per_frame - 1;
      }

      // Process payload data with correct symbol index
      process_payload_data(d_derot_in, out);

      // noutput_items should be 1 in this case
      return 1;
    }

    dvbt_reference_signals::sptr
    dvbt_reference_signals::make(int itemsize, int ninput, int noutput, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, \
        dvb_code_rate_t code_rate_HP, dvb_code_rate_t code_rate_LP, \
        dvb_guardinterval_t guard_interval, dvbt_transmission_mode_t transmission_mode, \
        int include_cell_id, int cell_id)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_reference_signals_impl(itemsize, ninput, \
            noutput, constellation, hierarchy, code_rate_HP, code_rate_LP, \
            guard_interval, transmission_mode, include_cell_id, cell_id));
    }

    /*
     * The private constructor
     */
    dvbt_reference_signals_impl::dvbt_reference_signals_impl(int itemsize, int ninput, int noutput, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvb_code_rate_t code_rate_HP,\
          dvb_code_rate_t code_rate_LP, dvb_guardinterval_t guard_interval,\
          dvbt_transmission_mode_t transmission_mode, int include_cell_id, int cell_id)
      : block("dvbt_reference_signals",
          io_signature::make(1, 1, itemsize * ninput),
          io_signature::make(1, 1, itemsize * noutput)),
          config(constellation, hierarchy, code_rate_HP, code_rate_LP, \
              guard_interval, transmission_mode, include_cell_id, cell_id),
    d_pg(config)
    {
      d_ninput = ninput;
      d_noutput = noutput;
    }

    /*
     * Our virtual destructor.
     */
    dvbt_reference_signals_impl::~dvbt_reference_signals_impl()
    {
    }

    void
    dvbt_reference_signals_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    dvbt_reference_signals_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        d_pg.update_output(&in[i * d_ninput], &out[i * d_noutput]);
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


