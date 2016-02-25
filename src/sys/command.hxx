#ifndef DBL_SYS_COMMAND_HXX
#define DBL_SYS_COMMAND_HXX

#include <string>
#include <iostream>
#include <cstdio>
#include <memory>


namespace dbl {
namespace sys {

int run_command(const std::string& cmd,	std::string& result);

} // sys
} // dbl

#endif
