#ifndef DBLCCLIENT_NET_SERVICE_RESPONSE_HXX
#define DBLCCLIENT_NET_SERVICE_RESPONSE_HXX

#include <string>
#include <json/json.h>

namespace dblclient {
namespace net {

class ServiceResponse
{
public:
	ServiceResponse() = delete;
	explicit ServiceResponse(const std::string& raw_response);

	bool is_ok() const;

	const Json::Value& get_data() const;
private:
	Json::Value data_;
	bool is_ok_;
};

} // net
} // dblclient

#endif
