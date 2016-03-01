#ifndef DBL_SERVICE_SERVER_SERVER_HXX
#define DBL_SERVICE_SERVER_SERVER_HXX


#include "core/api.hxx"
#include "core/common.hxx"

#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>


namespace dbl {
namespace service {
namespace server {

template<class ConnectionType>
class Server
{
public:
	Server() = delete;
	Server(std::shared_ptr<core::Api> api, short port);
	virtual ~Server();

	virtual void run() final;
	virtual void stop() final;

private:
	std::shared_ptr<core::Api> api_;
	short port_;
	std::unique_ptr<boost::asio::io_service> io_service_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::acceptor acceptor_;

	void accept();
};


template<class ConnectionType>
Server<ConnectionType>::Server(std::shared_ptr<core::Api> api, short port)
	: api_(api),
	  port_(port),
	  io_service_(new boost::asio::io_service()),
	  socket_(*io_service_),
	  acceptor_(
		  *io_service_,
		  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}

template<class ConnectionType>
Server<ConnectionType>::~Server()
{
	this->stop();
	io_service_.reset();
}

template<class ConnectionType>
void Server<ConnectionType>::run()
{
	accept();
	LOG(INFO) << "Activating server on port " << port_;
	io_service_->run();
}

template<class ConnectionType>
void Server<ConnectionType>::stop()
{
	if(!io_service_->stopped()) {
		LOG(INFO) << "Deactivating server on port " << port_;
		io_service_->stop();
	}
}

template<class ConnectionType>
void Server<ConnectionType>::accept()
{
	acceptor_.async_accept(
		socket_,
		[this](boost::system::error_code error) {
			if(!error) {
				std::make_shared<ConnectionType>(api_, std::move(socket_))->handle();
			}
			else {
				LOG(ERROR) << "Server accept() failed";
			}
			accept();
		}
	);
}

} // server
} // service
} // dbl

#endif
