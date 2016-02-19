#ifndef DBL_SERVICE_SERVER_HXX
#define DBL_SERVICE_SERVER_HXX

#include <memory>
#include <utility>
#include <boost/asio.hpp>

namespace dbl {

using boost::asio::ip::tcp;
using boost::asio::io_service;

class ServiceServer
{
public:
	ServiceServer() = delete;
	ServiceServer(io_service& io_service, short port);
	~ServiceServer() = default;

private:
	void accept();

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

} // dbl

#endif
