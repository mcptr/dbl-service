#ifndef DBL_NET_NET_HXX
#define DBL_NET_NET_HXX

#include <string>
#include <memory>
#include <curl/curl.h>

namespace dbl {
namespace net {

class HTTPRequest
{
public:
	bool ssl_verify_peer = true;
	bool ssl_verify_host = true;

	bool fetch(const std::string& url, long ok_code = 0);

	long get_status_code() const;
	bool is_code(long code) const;

	inline const std::string& get_result()
	{
		return result_;
	}

	inline const std::string& get_error()
	{
		return error_;
	}

private:
	std::string result_;
	std::string error_;
	long status_code_ = 0;

	static int writer(char *data, std::size_t size,
					  std::size_t nmemb, std::string *writerData);
};

} // net
} // dbl

#endif
