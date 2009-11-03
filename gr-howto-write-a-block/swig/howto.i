/* -*- c++ -*- */

%include "gnuradio.i"			// the common stuff

%{
#include "howto_square_ff.h"
#include "howto_square2_ff.h"
%}

// ----------------------------------------------------------------

/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access howto_square_ff from python as howto.square_ff
 */
GR_SWIG_BLOCK_MAGIC(howto,square_ff);

howto_square_ff_sptr howto_make_square_ff ();

class howto_square_ff : public gr_block
{
private:
  howto_square_ff ();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(howto,square2_ff);

howto_square2_ff_sptr howto_make_square2_ff ();

class howto_square2_ff : public gr_sync_block
{
private:
  howto_square2_ff ();
};
