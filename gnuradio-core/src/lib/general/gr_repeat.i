/* -*- c++ -*- */

GR_SWIG_BLOCK_MAGIC(gr,repeat);

gr_repeat_sptr gr_make_repeat(size_t itemsize, int interp);

class gr_repeat : public gr_sync_interpolator
{
private:
  gr_repeat();
};
