#include "connection.hxx"
#include <boost/bind.hpp>

namespace dbl {
namespace service {
namespace server {

Connection::Connection(std::shared_ptr<core::Api> api,
					   boost::asio::ip::tcp::socket socket)
	: api_(api),
	  socket_(std::move(socket))
{
}

void Connection::handle()
{
	read();
}

void Connection::read()
{
	using boost::system::error_code;

	auto self(shared_from_this());
	socket_.async_read_some(
		boost::asio::buffer(data_, MAX_REQUEST_SIZE),
		[this, self](error_code error, std::size_t len) {
			if(error) {
				if(error != boost::asio::error::connection_reset) {
					LOG(ERROR) << "Handler error: " << error;
				}
			}
			else {
				std::string request;

				std::copy(
					data_.begin(),
					data_.begin() + len,
					std::back_inserter(request)
				);

				std::string response;

				try {
					this->process_request(request, response);
				}
				catch(const std::exception& e) {
					LOG(ERROR) << "Processing request failed: "
							   << e.what();
					LOG(ERROR) << request;
					response = "Processing failed.";
					if(this->api_->config.is_debug) {
						response.append("\n");
						response.append(e.what());
					}
				}

				this->respond(response);
			}
		}
	);
}

void Connection::respond(const std::string& response)
{
	using boost::system::error_code;

	auto self(shared_from_this());
	boost::asio::async_write(
		socket_, boost::asio::buffer(response, response.length()),
		[this, self](error_code error, std::size_t /*len*/) {
			if(!error) {
				read();
			}
		}
	);
}

} // server
} // service
} //dbl
