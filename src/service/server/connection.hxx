#ifndef DBL_SERVICE_SERVER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_CONNECTION_HXX

#include "core/rtapi.hxx"

#include <string>
#include <memory>
#include <boost/asio.hpp>

namespace dbl {
namespace service {

constexpr int CONNECTION_BUFFER_SIZE = 2048;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection() = delete;
	Connection(std::shared_ptr<dbl::RTApi> api,
			   boost::asio::ip::tcp::socket socket);
	virtual ~Connection() = default;

	virtual void handle();

protected:
	std::shared_ptr<dbl::RTApi> api_;
	boost::asio::ip::tcp::socket socket_;
	char request_data_[CONNECTION_BUFFER_SIZE];

	std::string response_;

	void read();
	void respond();
	virtual bool process_request() = 0;
};

} // service
} //dbl

#endif
