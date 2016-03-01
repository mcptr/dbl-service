#ifndef DBL_SCRIPT_HXX
#define DBL_SCRIPT_HXX

#if defined(__linux)
#include "dbl/script/unix.hxx"
#elif defined(__freebsd)
#include "dbl/script/unix.hxx"
#elif defined(_WIN32)
#include "dbl/script/windows.hxx"
#endif

#endif
