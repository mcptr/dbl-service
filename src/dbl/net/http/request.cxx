#include "request.hxx"
#include "response.hxx"
#include "dbl/core/common.hxx"

#include <vector>
#include <cerrno>
#include <cstring>
#include <ctime>

namespace dbl {
namespace net {
namespace http {

Request::Request(const std::string& url)
	: url_(url),
	  curl_ptr_(CurlPtr_t(curl_easy_init(), curl_deleter))
{
}

std::size_t Request::writer(char *data,
							   std::size_t size,
							   std::size_t nmemb,
							   std::string *writer_data)
{
	if(writer_data == nullptr) {
		return 0;
	}

	writer_data->append(data, (size * nmemb));
	return (size * nmemb);
}

const std::string& Request::get_error() const
{
	return error_;
}
std::unique_ptr<Response> Request::head()
{
	std::unique_ptr<Response> response_ptr(new Response());
	prepare(*response_ptr);
	prepare_no_body();
	bool ok = execute(*response_ptr);
	if(!ok) {
		LOG(ERROR) << "HEAD request failed:" << url_;
		response_ptr.reset();
	}
	
	return std::move(response_ptr);
}

std::unique_ptr<Response> Request::fetch()
{
	std::unique_ptr<Response> response_ptr(new Response());
	prepare(*response_ptr);
	bool ok = execute(*response_ptr);
	if(!ok) {
		LOG(ERROR) << "Fetching failed:" << url_;
		response_ptr.reset(nullptr);
	}
	return std::move(response_ptr);
}

bool Request::execute(Response& r)
{
	CURLcode res = curl_easy_perform(curl_ptr_.get());
	bool ok = (res == CURLE_OK);

	if(!ok) {
		error_ = curl_easy_strerror(res);
		LOG(ERROR) << error_;
	}
	else {
		curl_easy_getinfo(
			curl_ptr_.get(),
			CURLINFO_RESPONSE_CODE,
			&(r.status_code_)
		);
	}

	r.parse_headers();
	curl_slist_free_all(curl_headers_);
	return ok;
}


void Request::set_header(const std::string& k,
							 const std::string& v)
{
	headers_[k] = v;
}

void Request::set_if_modified_since(long tstamp)
{
	std::time_t stamp(tstamp);
    struct tm* t = localtime(&stamp); 
	char buf[64];
	strftime(buf, 64, "%a, %d %b %Y %H:%M:%S %z", t);
	set_header("If-Modified-Since", buf);
}


bool Request::prepare(Response& r)
{
	bool ok = false;

	ok = prepare_url();
	if(!ok) {
		LOG(ERROR) << "prepare_url() failed";
	}
	ok = prepare_follow_location();
	if(!ok) {
		LOG(ERROR) << "prepare_follow_location() failed";
	}

	ok = prepare_ssl_verify();
	if(!ok) {
		LOG(ERROR) << "prepare_ssl_verify() failed";
	}

	// ok = prepare_headers();
	// if(!ok) {
	// 	LOG(ERROR) << "prepare_headers() failed";
	// }

	ok = prepare_header_writer(r);
	if(!ok) {
		LOG(ERROR) << "prepare_header_writer() failed";
	}

	ok = prepare_body_writer(r);
	if(!ok) {
		LOG(ERROR) << "prepare_body_writer() failed";
	}

	return true;
}

bool Request::prepare_url()
{
	CURLcode res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_URL,
		url_.c_str()
	);

	if(res != CURLE_OK) {
		LOG(ERROR) << "CURLOPT_URL: " << curl_easy_strerror(res);
		return false;
	}
	return true;
}

bool Request::prepare_follow_location()
{
	CURLcode res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_FOLLOWLOCATION,
		1L
	);

	if(res != CURLE_OK) {
		LOG(ERROR) << "CURLOPT_FOLLOWLOCATION: "
				   << curl_easy_strerror(res);
		return false;
	}
	return true;
}

bool Request::prepare_ssl_verify()
{
	CURLcode res;

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_SSL_VERIFYPEER,
		(ssl_verify_peer ? 1L : 0L)
	);

	if(res != CURLE_OK) {
		LOG(ERROR) << "CURLOPT_SSL_VERIFYPEER: "
				   << curl_easy_strerror(res);
		return false;
	}

	if(!ssl_verify_host) {
		res = curl_easy_setopt(
			curl_ptr_.get(),
			CURLOPT_SSL_VERIFYHOST,
			0L
		);
	}

	if(res != CURLE_OK) {
		LOG(ERROR) << "CURLOPT_SSL_VERIFYHOST: "
				   << curl_easy_strerror(res);
		return false;
	}

	return true;
}

bool Request::prepare_headers()
{
	headers_["X-Client-Software"] = "DNSBlocker/Updater";

	for(auto const& h : headers_) {
		std::string h_value = h.first + ":" + h.second;
		curl_headers_ = curl_slist_append(curl_headers_, h_value.c_str());
	}
	// FIXME
	return true;
}

bool Request::prepare_header_writer(Response& r)
{
	CURLcode res;

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_HEADERFUNCTION,
		writer
	);

	if(res != CURLE_OK)	{
		LOG(ERROR) << "CURLOPT_HEADERFUNCTION: "
				   << curl_easy_strerror(res);
		return false;
	}

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_HEADERDATA,
		&(r.raw_headers_)
	);

	if(res != CURLE_OK)	{
		LOG(ERROR) << "CURLOPT_HEADERDATA: "
				   << curl_easy_strerror(res);
		return false;
	}

	return true;
}

bool Request::prepare_no_body()
{
	CURLcode res;

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_NOBODY,
		1
	);

	if(res != CURLE_OK)	{
		LOG(ERROR) << "CURLOPT_NOBODY: "
				   << curl_easy_strerror(res);
		return false;
	}

	return true;
}

bool Request::prepare_body_writer(Response& r)
{
	CURLcode res;

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_WRITEFUNCTION,
		writer
	);

	if(res != CURLE_OK)	{
		LOG(ERROR) << "CURLOPT_WRITEFUNCTION: "
				   << curl_easy_strerror(res);
		return false;
	}

	res = curl_easy_setopt(
		curl_ptr_.get(),
		CURLOPT_WRITEDATA,
		&(r.body_)
	);

	if(res != CURLE_OK)	{
		LOG(ERROR) << "CURLOPT_WRITEDATA: "
				   << curl_easy_strerror(res);
		return false;
	}

	return true;
}

} // http
} // net
} // dbl
