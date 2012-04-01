/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access fcd_source_c from python as fcd.source_c
 */
GR_SWIG_BLOCK_MAGIC(fcd,source_c);

fcd_source_c_sptr fcd_make_source_c (const std::string device_name = "");

class fcd_source_c : public gr_hier_block2
{
public:
  void set_freq(float freq);
  void set_freq_khz(int freq);
  void set_lna_gain(float gain);
  void set_mixer_gain(float gain);
  void set_freq_corr(int ppm);
  void set_dc_corr(double dci, double dcq);
  void set_iq_corr(double gain, double phase);
};
