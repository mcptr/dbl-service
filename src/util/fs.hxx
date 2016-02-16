#ifndef DBL_UTIL_FS_HXX
#define DBL_UTIL_FS_HXX

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace dbl {

std::string find_executable(const std::string& name);

} // dbl

#endif
