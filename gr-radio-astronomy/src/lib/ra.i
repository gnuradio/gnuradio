/* -*- c++ -*- */

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include <stdexcept>
%}

// We'll need something like this if we add C++ blocks...
// See the gr-howto-write-a-block tarball for examples

// ----------------------------------------------------------------

/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access ra_square_ff from python as ra.square_ff
 */
// GR_SWIG_BLOCK_MAGIC(ra,square_ff);

// ra_square_ff_sptr ra_make_square_ff ();
// 
// class ra_square_ff : public gr_block
// {
// private:
//   ra_square_ff ();
// };
// 
// // ----------------------------------------------------------------
// 
// GR_SWIG_BLOCK_MAGIC(ra,square2_ff);
// 
// ra_square2_ff_sptr ra_make_square2_ff ();
// 
// class ra_square2_ff : public gr_sync_block
// {
// private:
//   ra_square2_ff ();
// };
