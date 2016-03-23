#ifndef DBLCLIENT_NET_SERVICE_CONNECTION_HXX
#define DBLCLIENT_NET_SERVICE_CONNECTION_HXX

#include "service_request.hxx"
#include "service_response.hxx"
#include "dblclient/error.hxx"

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace dblclient {
namespace net {

namespace ba = boost::asio;
namespace bpt = boost::posix_time;
namespace bip = boost::asio::ip;

class ServiceConnection
{
public:
	ServiceConnection();
	explicit ServiceConnection(const ServiceConnection&) = delete;

	void open(
		const std::string& address = "127.0.0.1",
		int port = 7654,
		bpt::time_duration timeout = bpt::seconds(5)
	) throw (DBLClientError);

	void write(
		const std::string& data,
		bpt::time_duration timeout = bpt::seconds(5)
	) throw (DBLClientError);

	void read(
		std::string& result,
		bpt::time_duration timeout = bpt::seconds(5)
	) throw (DBLClientError);

	std::unique_ptr<ServiceResponse>
	execute(const ServiceRequest& req) throw (DBLClientError);


private:
	ba::io_service io_service_;
	bip::tcp::socket socket_;
	ba::deadline_timer deadline_;

	const std::string eof_marker_ = "\r\n";

	void check_deadline();
};

} // net
} // dblclient

#endif
