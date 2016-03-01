#ifndef DBL_SERVICE_SERVER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_CONNECTION_HXX

#include "dbl/core/api.hxx"

#include <string>
#include <sstream>
#include <memory>
#include <boost/asio.hpp>

namespace dbl {
namespace service {
namespace server {

constexpr int MAX_REQUEST_SIZE = 1024 * 1024;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection() = delete;
	Connection(std::shared_ptr<core::Api> api,
			   boost::asio::ip::tcp::socket socket);
	virtual ~Connection() = default;

	virtual void handle();

protected:
	std::shared_ptr<core::Api> api_;
	boost::asio::ip::tcp::socket socket_;

	std::string buffer_;
	boost::array<char, MAX_REQUEST_SIZE> data_;

	void read();
	virtual void respond(const std::string& response) final;

	virtual void process_request(const std::string& request,
								 std::string& response) = 0;
};

} // server
} // service
} //dbl

#endif
