#ifndef DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX
#define DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX

#include "connection.hxx"
#include <memory>

namespace dbl {
namespace service {

class ServiceConnection : public Connection

{
public:
	ServiceConnection() = delete;
	ServiceConnection(std::shared_ptr<dbl::RTApi> api,
					  boost::asio::ip::tcp::socket socket);
	virtual ~ServiceConnection() = default;

protected:
	virtual bool process_request();
};

}
} // dbl

#endif
