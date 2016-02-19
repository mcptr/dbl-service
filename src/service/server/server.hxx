#ifndef DBL_SERVICE_SERVER_SERVER_HXX
#define DBL_SERVICE_SERVER_SERVER_HXX

#include "connection.hxx"

#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>


namespace dbl {
namespace service {

class Server
{
public:
	Server(short port = 7654);
	~Server() = default;

	virtual void run() final;
	virtual void stop() final;

private:
	std::unique_ptr<boost::asio::io_service> io_service_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::acceptor acceptor_;

	void accept();
};

} // service
} // dbl

#endif

