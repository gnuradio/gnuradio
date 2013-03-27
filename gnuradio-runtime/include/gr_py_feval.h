#include <gruel/pmt.h>

class gr_py_feval_dd : public gr_feval_dd
{
 public:
  double calleval(double x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval_cc : public gr_feval_cc
{
 public:
  gr_complex calleval(gr_complex x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval_ll : public gr_feval_ll
{
 public:
  long calleval(long x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval : public gr_feval
{
 public:
  void calleval()
  {
    ensure_py_gil_state _lock;
    eval();
  }
};

class gr_py_feval_p : public gr_feval_p
{
 public:
  void calleval(pmt::pmt_t x)
  {
    ensure_py_gil_state _lock;
    eval(x);
  }
};
