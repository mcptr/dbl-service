#include "service_request.hxx"

namespace dblclient {
namespace net {

ServiceRequest::ServiceRequest(const std::string& cmd)
	: cmd_(cmd)
{
}

std::unique_ptr<ServiceResponse> ServiceRequest::run() const
{
	std::string raw_response;
	std::unique_ptr<ServiceResponse> ptr(
		new ServiceResponse(raw_response)
	);


	return std::move(ptr);
}

} // net
} // dblclient
