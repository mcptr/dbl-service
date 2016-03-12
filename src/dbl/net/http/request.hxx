#ifndef DBL_NET_HTTP_REQUEST_HXX
#define DBL_NET_HTTP_REQUEST_HXX

#include "dbl/types/http.hxx"

#include <string>
#include <memory>
#include <curl/curl.h>


namespace dbl {
namespace net {
namespace http {

// fwd
class Response;

auto curl_deleter = [](CURL* ptr) {
	if(ptr) {
		curl_easy_cleanup(ptr);
	}
};

class Request
{
public:
	typedef std::unique_ptr<CURL, decltype(curl_deleter)> CurlPtr_t;

	Request() = delete;
	Request(const std::string& url);
	~Request() = default;

	bool ssl_verify_peer = true;
	bool ssl_verify_host = true;

	const std::string& get_error() const;
	void set_header(const std::string& k, const std::string& v);
	void set_if_modified_since(long tstamp);

	std::unique_ptr<Response> head();
	std::unique_ptr<Response> fetch();

private:
	const std::string url_;
	CurlPtr_t curl_ptr_;

	types::http::Headers_t headers_;
	std::string error_;
	struct curl_slist* curl_headers_ = nullptr;

	static std::size_t writer(
		char* data,
		std::size_t size,
		std::size_t nmemb,
		std::string *writerData);

	bool prepare(Response& r);

	bool prepare_url();
	bool prepare_follow_location();
	bool prepare_ssl_verify();
	bool prepare_headers();
	bool prepare_header_writer(Response& r);
	bool prepare_no_body();
	bool prepare_body_writer(Response& r);

	bool execute(Response& r);
};

} // http
} // net
} // dbl

#endif
