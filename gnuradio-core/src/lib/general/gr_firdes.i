/* -*- C++ -*- */

/*!
 * \brief Finite Impulse Response (FIR) filter design functions.
 */

%rename(firdes) gr_firdes;

class gr_firdes {
 public:

  enum win_type {
    WIN_HAMMING = 0,	// max attenuation 53 dB
    WIN_HANN = 1,	// max attenuation 44 dB
    WIN_BLACKMAN = 2,	// max attenuation 74 dB
    WIN_RECTANGULAR = 3,
    WIN_KAISER = 4      // max attenuation variable with beta, google it
  };

  // ... class methods ...

  /*!
   * \brief use "window method" to design a low-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */
  static std::vector<float>
  low_pass (double gain,
	    double sampling_freq,
	    double cutoff_freq,		// Hz center of transition band
	    double transition_width,	// Hz width of transition band
	    win_type window = WIN_HAMMING,
	    double beta = 6.76 		// used only with Kaiser
	    ) throw(std::out_of_range);

  /*!
   * \brief use "window method" to design a high-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */
  static std::vector<float>
  high_pass (double gain,
	     double sampling_freq,
	     double cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,
	     double beta = 6.76			// used only with Kaiser
	     ) throw(std::out_of_range); 

  /*!
   * \brief use "window method" to design a band-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */
  static std::vector<float>
  band_pass (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz center of transition band
	     double high_cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,
	     double beta = 6.76			// used only with Kaiser
	     ) throw(std::out_of_range); 


  /*!
   * \brief use "window method" to design a band-reject FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<gr_complex>
  complex_band_pass (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz center of transition band
	     double high_cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,	// used only with Kaiser
	     double beta = 6.76
	     ) throw(std::out_of_range); 


  /*!
   * \brief use "window method" to design a band-reject FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  band_reject (double gain,
	       double sampling_freq,
	       double low_cutoff_freq,		// Hz center of transition band
	       double high_cutoff_freq,		// Hz center of transition band
	       double transition_width,		// Hz width of transition band
	       win_type window = WIN_HAMMING,   // used only with Kaiser
	       double beta = 6.76
	       ) throw(std::out_of_range);

  /*!\brief design a Hilbert Transform Filter
   *
   * \p ntaps:                  Number of taps, must be odd
   * \p window_type:            What kind of window to use
   * \p beta:                   Only used for Kaiser
   */
  static std::vector<float>
  hilbert (unsigned int ntaps = 19,
	   win_type windowtype = WIN_RECTANGULAR,
	   double beta = 6.76
	   ) throw(std::out_of_range);
   
  /*!
   * \brief design a Root Cosine FIR Filter (do we need a window?)
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p symbol rate:		symbol rate, must be a factor of sample rate
   * \p alpha:		        excess bandwidth factor
   * \p ntaps:		        number of taps
   */
  static std::vector<float>
  root_raised_cosine (double gain,
		      double sampling_freq,
		      double symbol_rate,       // Symbol rate, NOT bitrate (unless BPSK)
		      double alpha,             // Excess Bandwidth Factor
		      int ntaps) throw(std::out_of_range);

  /*!
   * \brief design a Gaussian filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p symbols per bit:	symbol rate, must be a factor of sample rate
   * \p bt:                     BT bandwidth time product
   * \p ntaps:		        number of taps
   */
  static std::vector<float>
  gaussian (double gain,
	    double spb,       
	    double bt,              // Bandwidth to bitrate ratio
	    int ntaps) throw(std::out_of_range);

  /*!
   * Return window given type, ntaps and optional beta.
   */
  static std::vector<float> gr_firdes::window (win_type type, int ntaps, double beta) 
    throw(std::runtime_error);
};
