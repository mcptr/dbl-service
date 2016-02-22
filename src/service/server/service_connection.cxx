#include "service_connection.hxx"

#include <string>
#include <vector>


namespace dbl {
namespace service {


ServiceConnection::ServiceConnection(
	std::shared_ptr<dbl::RTApi> api,
	boost::asio::ip::tcp::socket socket)
	: Connection(api, std::move(socket))
{
}

bool ServiceConnection::process_request()
{
	response_.clear();
	return true;
}

} // service
} // dbl

