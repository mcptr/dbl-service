#include "http.hxx"
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>

namespace dbl {
namespace util {
namespace http {

void parse_headers(const std::string& raw,
				   dbl::types::http::Headers_t& headers)
{
	std::vector<std::string> lines;
	boost::algorithm::split(lines, raw, boost::is_any_of("\r"));
	for(auto const& line : lines) {
		std::size_t idx = line.find(':', 0);
		if(idx != std::string::npos) {
			headers.insert(
				std::make_pair(
					boost::algorithm::trim_copy(line.substr(0, idx)), 
					boost::algorithm::trim_copy(line.substr(idx + 1))
				)
			);
		}
	}

}

} // http
} // util
} // dbl
