#include "server.hxx"

namespace dbl {
namespace service {

Server::Server(short port)
	: io_service_(new boost::asio::io_service()),
	  socket_(*io_service_),
	  acceptor_(
		  *io_service_,
		  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}

void Server::run()
{
	accept();
	io_service_->run();
}

void Server::stop()
{
	io_service_->stop();
}

void Server::accept()
{
	acceptor_.async_accept(
		socket_,
		[this](boost::system::error_code error) {
			if(!error) {
				std::make_shared<Connection>(std::move(socket_))->handle();
			}

			accept();
		}
	);
}

} // service
} //dbl
