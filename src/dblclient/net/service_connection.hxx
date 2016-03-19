#ifndef DBLCLIENT_NET_SERVICE_CONNECTION_HXX
#define DBLCLIENT_NET_SERVICE_CONNECTION_HXX

#include "service_request.hxx"
#include "service_response.hxx"
#include "dblclient/error.hxx"

#include <string>
#include <memory>
#include <boost/asio.hpp>

namespace dblclient {
namespace net {

class ServiceConnection
{
public:
	ServiceConnection();
	explicit ServiceConnection(const ServiceConnection&) = delete;
	explicit ServiceConnection(const std::string& address, int port);

	void open() throw(DBLClientError);

	std::unique_ptr<ServiceResponse>
	execute(const ServiceRequest& req);

private:
	const std::string address_;
	const int port_;
	boost::asio::ip::tcp::iostream stream_ = {};
};

} // net
} // dblclient

#endif
