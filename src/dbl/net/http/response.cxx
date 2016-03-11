#include "response.hxx"
#include "dbl/core/common.hxx"

#include <ctime>
#include <boost/algorithm/string.hpp>


namespace dbl {
namespace net {
namespace http {

long Response::get_status_code() const
{
	return status_code_;
}

bool Response::is_code(long code) const
{
	return (status_code_ == code);
}

std::string Response::get_header(const std::string& name) const
{
	std::unordered_map<std::string, std::string>::const_iterator it;
	it = headers_.find(name);
	return (it != headers_.end() ? it->second : "");
}

long Response::get_last_modified() const
{
	const std::string lm = get_header("Last-Modified");

	if(lm.empty()) {
		return 0;
	}

	struct tm t;
	// Welcome to the 21st century where dates are strings, years are
	// off by 1900, months and days may be counted starting from 0
	// and timezones are a political matter.
	// TODO: Revise looping over several possible formats.
	if(strptime(lm.c_str(), "%a, %d %b %Y %H:%M:%S", &t) == nullptr) {
		LOG(ERROR) << "Failed to parse Last-Modified header";
		// If it failed - it failed.
		return 0;
	}
	
	std::time_t epoch(mktime(&t));
	return epoch;
}

const std::string& Response::get_data() const
{
	return body_;
}

void Response::parse_headers()
{
	std::vector<std::string> lines;
	boost::algorithm::split(lines, raw_headers_, boost::is_any_of("\r"));
	for(auto const& line : lines) {
		std::size_t idx = line.find(':', 0);
		if(idx != std::string::npos) {
			headers_.insert(
				std::make_pair(
					boost::algorithm::trim_copy(line.substr(0, idx)), 
					boost::algorithm::trim_copy(line.substr(idx + 1))
				)
			);
		}
	}
}

} // http
} // net
} // dbl
