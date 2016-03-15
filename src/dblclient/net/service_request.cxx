#include "service_request.hxx"

namespace dblclient {
namespace net {

ServiceRequest::ServiceRequest(const std::string& cmd)
	: cmd_(cmd)
{
}

std::string ServiceRequest::to_string() const
{
	Json::Value payload;
	payload["command"] = cmd_;
	payload["data"] = params_;
	return payload.toStyledString();
}

} // net
} // dblclient
