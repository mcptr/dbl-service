#include "service_connection.hxx"

namespace dblclient {
namespace net {

ServiceConnection::ServiceConnection()
	: ServiceConnection("127.0.0.1", 7654)
{
}

ServiceConnection::ServiceConnection(const std::string& address, short port)
	: address_(address),
	  port_(port)
{
}

void ServiceConnection::open() throw(DBLClientError)
{
	try {
		stream_.reset(
			new boost::asio::ip::tcp::iostream(address_, std::to_string(port_))
		);
	}
	catch(const std::exception& e) {
		std::string msg = "Cannot connect to service: ";
		msg.append(e.what());
		throw DBLClientError();
	}
}

std::unique_ptr<ServiceResponse>
ServiceConnection::execute(const ServiceRequest& req) const
{
	*stream_ << req.to_string();

	std::string raw_response;
	*stream_ >> raw_response;

	std::unique_ptr<ServiceResponse> ptr(
		new ServiceResponse(raw_response)
	);

	return std::move(ptr);
}

} // net
} // dblclient
