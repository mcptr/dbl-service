#include "net.hxx"
#include "dbl/core/common.hxx"

#include <cerrno>
#include <cstring>

namespace dbl {
namespace net {

int HTTPRequest::writer(char *data, std::size_t size,
						std::size_t nmemb, std::string *writerData)
{
	if(writerData == nullptr) {
		return 0;
	}
	writerData->append(data, (size * nmemb));
 	return (size * nmemb);
}

bool HTTPRequest::fetch(const std::string& url, long ok_code)
{
	auto deleter = [](CURL* ptr) {
		if(ptr) {
			curl_easy_cleanup(ptr);
		}
	};

	std::unique_ptr<CURL, decltype(deleter)> ptr(
		curl_easy_init(), deleter
	);

	bool ok = false;

	if(ptr) {
		CURLcode res;

		res = curl_easy_setopt(ptr.get(), CURLOPT_URL, url.c_str());
		if(res != CURLE_OK) {
			LOG(ERROR) << "CURLOPT_URL: " << curl_easy_strerror(res);
			return false;
		}

		res = curl_easy_setopt(ptr.get(), CURLOPT_FOLLOWLOCATION, 1L);
		if(res != CURLE_OK) {
			LOG(ERROR) << "CURLOPT_FOLLOWLOCATION: "
					   << curl_easy_strerror(res);
			return false;
		}

		res = curl_easy_setopt(
			ptr.get(),
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
				ptr.get(),
				CURLOPT_SSL_VERIFYHOST,
				0L
			);
		}

		if(res != CURLE_OK) {
			LOG(ERROR) << "CURLOPT_SSL_VERIFYHOST: "
					   << curl_easy_strerror(res);
			return false;
		}

		res = curl_easy_setopt(
			ptr.get(),
			CURLOPT_WRITEFUNCTION,
			writer
		);

		if(res != CURLE_OK)	{
			LOG(ERROR) << "CURLOPT_WRITEFUNCTION: "
					   << curl_easy_strerror(res);
			return false;
		}

		res = curl_easy_setopt(ptr.get(), CURLOPT_WRITEDATA, &result_);
		if(res != CURLE_OK)	{
			LOG(ERROR) << "CURLOPT_WRITEDATA: "
					   << curl_easy_strerror(res);
			return false;
		}

		res = curl_easy_perform(ptr.get());
		ok = (res == CURLE_OK);

		if(!ok) {
			error_ = curl_easy_strerror(res);
			LOG(ERROR) << error_;
		}
		else {
			curl_easy_getinfo(
				ptr.get(),
				CURLINFO_RESPONSE_CODE,
				&status_code_
			);
		}
	}

	ok = (ok && (ok_code ? is_code(ok_code) : true));
	LOG(DEBUG) << "HTTP " << status_code_ << ": " << url;
	return ok;
}

long HTTPRequest::get_status_code() const
{
	return status_code_;
}

bool HTTPRequest::is_code(long code) const
{
	return (status_code_ == code);
}


} // net
} // dbl
