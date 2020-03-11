#ifndef _MSC_VER // [
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif // _MSC_VER ]

#ifndef _MSC_SYS_TIME_H_
#define _MSC_SYS_TIME_H_

#include < time.h >

#if _MSC_VER < 1900
struct timespec {
    time_t tv_sec; /*! \brief Seconds since 00:00:00 GMT, 1 January 1970 */
    long tv_nsec;  /*! \brief Additional nanoseconds since tv_sec */
};
#endif

#endif //_MSC_SYS_TIME_H_
