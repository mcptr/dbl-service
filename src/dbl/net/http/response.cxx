#include "response.hxx"
#include "dbl/core/common.hxx"
#include "dbl/util/http.hxx"

#include <ctime>


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
	dbl::util::http::parse_headers(raw_headers_, headers_);
}

} // http
} // net
} // dbl
