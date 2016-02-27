#ifndef DBL_UTIL_FS_HXX
#define DBL_UTIL_FS_HXX

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace dbl {
namespace util {
namespace fs {

std::string find_executable(const std::string& name);

} // fs
} // util
} // dbl

#endif
