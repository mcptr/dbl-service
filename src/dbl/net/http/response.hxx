#ifndef DBL_NET_HTTP_RESPONSE_HXX
#define DBL_NET_HTTP_RESPONSE_HXX

#include "types.hxx"
#include <string>


namespace dbl {
namespace net {
namespace http {

class Response
{
public:
	friend class Request;

	long get_status_code() const;
	bool is_code(long code) const;
	std::string get_header(const std::string& name) const;
	long get_last_modified() const;
	const std::string& get_data() const;

	void parse_headers();

private:
	std::string raw_headers_;
	std::string body_;
	long status_code_ = 0;

	Headers_t headers_;
};

} // http
} // net
} // dbl

#endif
