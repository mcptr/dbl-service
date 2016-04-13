#include "service_connection.hxx"
#include <boost/lambda/lambda.hpp>

namespace dblclient {
namespace net {

ServiceConnection::ServiceConnection()
	: socket_(io_service_),
	  deadline_(io_service_)
{
	deadline_.expires_at(bpt::pos_infin);
	check_deadline();
}

ServiceConnection::~ServiceConnection()
{
	close();
}

void ServiceConnection::open(
	const std::string& address,
	int port,
	bpt::time_duration timeout) throw (DBLClientError)
{
	socket_.close();

	bip::tcp::endpoint ep(
		ba::ip::address::from_string(address),
		port
	);

	boost::system::error_code error = ba::error::would_block;

	deadline_.expires_from_now(timeout);

	socket_.async_connect(
		ep,
		boost::lambda::var(error) = boost::lambda::_1
	);

	do {
		io_service_.run_one();
	}
	while(error == ba::error::would_block);
	if(error || !socket_.is_open()) {
		throw DBLClientError("Cannot connect");
	}
}

void ServiceConnection::close()
{
	if(socket_.is_open()) {
		socket_.cancel();
		//socket_.shutdown(bip::tcp::socket::shutdown_both);
		socket_.close();
	}
}

void ServiceConnection::write(
	const std::string& data,
	bpt::time_duration timeout) throw (DBLClientError)
{
	boost::system::error_code error = ba::error::would_block;
	deadline_.expires_from_now(timeout);
	ba::async_write(
		socket_,
		ba::buffer(data),
		boost::lambda::var(error) = boost::lambda::_1
	);

	do {
		io_service_.run_one();
	}
	while(error == ba::error::would_block);
	if(error) {
		throw DBLClientError(error.message());
	}
}

void ServiceConnection::read(
	std::string& result,
	bpt::time_duration timeout) throw (DBLClientError)
{
	boost::system::error_code ec = ba::error::would_block;
	std::size_t length = 0;

	deadline_.expires_from_now(timeout);

	ba::streambuf read_buffer;

	ba::async_read_until(
		socket_,
		read_buffer,
		eof_marker_,
		[&ec, &length](const boost::system::error_code error, std::size_t len) {
			ec = error;
			length = len;
		}
	);

	do {
		io_service_.run_one();
	}
	while(ec == ba::error::would_block);

	ba::streambuf::const_buffers_type bufs = read_buffer.data();
	result.clear();
	result.append(
		ba::buffers_begin(bufs),
		ba::buffers_begin(bufs) + (length - eof_marker_.length())
	);
}

std::unique_ptr<ServiceResponse>
ServiceConnection::execute(const ServiceRequest& req) throw(DBLClientError)
{
	write(req.to_string());

	std::string raw_response;
	read(raw_response);

	std::unique_ptr<ServiceResponse> ptr(
		new ServiceResponse(raw_response)
	);

	return std::move(ptr);
}

void ServiceConnection::check_deadline()
{
	if(deadline_.expires_at() <= ba::deadline_timer::traits_type::now()) {
		socket_.close();
		deadline_.expires_at(bpt::pos_infin);
	}

	deadline_.async_wait(
		std::bind(&ServiceConnection::check_deadline, this)
	);
}

} // net
} // dblclient
