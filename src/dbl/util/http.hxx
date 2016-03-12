#ifndef DBL_UTIL_HTTP_HXX
#define DBL_UTIL_HTTP_HXX

#include "dbl/types/http.hxx"
#include <string>

namespace dbl {
namespace util {
namespace http {

void parse_headers(const std::string& raw,
				   types::http::Headers_t& headers);

} // http
} // util
} // dbl

#endif
