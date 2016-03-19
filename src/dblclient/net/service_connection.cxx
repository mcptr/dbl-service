#include "service_connection.hxx"
#include <iostream>

namespace dblclient {
namespace net {

ServiceConnection::ServiceConnection()
	: ServiceConnection("127.0.0.1", 7654)
{
}

ServiceConnection::ServiceConnection(const std::string& address, int port)
	: address_(address),
	  port_(port)
{
}

void ServiceConnection::open() throw(DBLClientError)
{
	stream_.connect(address_, std::to_string(port_));
	if(!stream_) {
		throw DBLClientError(
			"Cannot connect to the service: " +
			address_ + ":" + std::to_string(port_)
		);
	}
}

std::unique_ptr<ServiceResponse>
ServiceConnection::execute(const ServiceRequest& req)
{
	stream_ << req.to_string();

	std::string raw_response;
	stream_ >> raw_response;

	std::cout << "RAW RESPONSE CONN: " << raw_response;
	std::unique_ptr<ServiceResponse> ptr(
		new ServiceResponse(raw_response)
	);

	return std::move(ptr);
}

} // net
} // dblclient
