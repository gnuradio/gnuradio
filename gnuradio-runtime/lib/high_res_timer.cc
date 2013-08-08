#include <gnuradio/high_res_timer.h>

#ifdef GNURADIO_HRT_USE_CLOCK_GETTIME
clockid_t gr::high_res_timer_source = CLOCK_THREAD_CPUTIME_ID;
#endif



