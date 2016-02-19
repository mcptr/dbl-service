#ifndef DBL_SERVICE_SERVER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_CONNECTION_HXX

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
	Connection(boost::asio::ip::tcp::socket socket);
	virtual ~Connection() = default;

	virtual void handle();

protected:
	std::string response_;
	virtual bool process_request();

private:
	boost::asio::ip::tcp::socket socket_;
	char request_data_[CONNECTION_BUFFER_SIZE];

	void read();
	void respond();
};

} // service
} //dbl

#endif
