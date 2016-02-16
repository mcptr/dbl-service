#ifndef DBL_SCRIPT_HXX
#define DBL_SCRIPT_HXX

#if defined(__linux)
#include "script/unix.hxx"
#elif defined(__freebsd)
#include "script/unix.hxx"
#elif defined(_WIN32)
#include "script/windows.hxx"
#endif

#endif
