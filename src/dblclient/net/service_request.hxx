#ifndef DBLCLIENT_NET_SERVICE_REQUEST_HXX
#define DBLCLIENT_NET_SERVICE_REQUEST_HXX

#include "service_response.hxx"
#include <string>
#include <memory>
#include <json/json.h>

namespace dblclient {
namespace net {

class ServiceRequest
{
public:
	ServiceRequest() = delete;
	explicit ServiceRequest(const std::string& cmd);
	~ServiceRequest() = default;

	template <class T>
	void set_parameter(const std::string& name, const T& value = T());

	std::string to_string() const;
private:
	const std::string cmd_;
	Json::Value params_;
};


template <class T>
void ServiceRequest::set_parameter(const std::string& name, const T& value)
{
	params_[name] = value;
}

} // net
} // dblclient

#endif
