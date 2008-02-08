/* -*- c++ -*- */

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "gpio_and_const_ss.h"
#include <stdexcept>
%}

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(gpio,and_const_ss)

gpio_and_const_ss_sptr gpio_make_and_const_ss (unsigned short k);

class gpio_and_const_ss : public gr_sync_block
{
 private:
  gpio_and_const_ss (unsigned short k);

 public:
  unsigned short k () const { return d_k; }
  void set_k (unsigned short k) { d_k = k; }
};
