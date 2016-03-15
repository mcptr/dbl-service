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
	ServiceConnection(const std::string& address, short port);
	ServiceConnection();

	void open() throw(DBLClientError);

	std::unique_ptr<ServiceResponse>
	execute(const ServiceRequest& req) const;

private:
	const std::string address_;
	const short port_;
	std::unique_ptr<boost::asio::ip::tcp::iostream> stream_;
};

} // net
} // dblclient

#endif
