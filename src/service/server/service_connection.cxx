#include "service_connection.hxx"

#include <string>
#include <vector>


namespace dbl {
namespace service {
namespace server {

ServiceConnection::ServiceConnection(
	std::shared_ptr<core::Api> api,
	boost::asio::ip::tcp::socket socket)
	: Connection(api, std::move(socket))
{
}

bool ServiceConnection::process_request()
{
	response_.clear();
	return true;
}

} // server
} // service
} // dbl
