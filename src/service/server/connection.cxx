#include "connection.hxx"

namespace dbl {
namespace service {

Connection::Connection(std::shared_ptr<dbl::RTApi> api,
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
		boost::asio::buffer(request_data_, CONNECTION_BUFFER_SIZE),
		[this, self](error_code error, std::size_t /* len */) {
			if(!error) {
				this->process_request();
				respond();
			}
		}
	);
}

void Connection::respond()
{
	using boost::system::error_code;

	auto self(shared_from_this());
	boost::asio::async_write(
		socket_, boost::asio::buffer(response_, response_.length()),
		[this, self](error_code error, std::size_t /*len*/) {
			if(!error) {
				read();
			}
		}
	);
}

} // service
} //dbl
