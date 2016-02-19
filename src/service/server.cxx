#include "server.hxx"


namespace dbl {

ServiceServer::ServiceServer(io_service& io_service, short port)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	  socket_(io_service)
{
	accept();
}

void ServiceServer::accept()
{
	using boost::system::error_code;
    acceptor_.async_accept(socket_,	[this](error_code err_code) {
			if(!err_code) {
				std::make_shared<session>(std::move(socket_))->start();
			}
			accept();
		}
	);
  }

} // dbl
