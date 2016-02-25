#ifndef DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX

#include "connection.hxx"

#include <memory>

namespace dbl {
namespace service {
namespace server {

class HTTPResponderConnection : public Connection

{
public:
	HTTPResponderConnection() = delete;
	HTTPResponderConnection(std::shared_ptr<core::Api> api,
							boost::asio::ip::tcp::socket socket);
	virtual ~HTTPResponderConnection() = default;

protected:
	virtual bool process_request();
};

} // server
} // service
} // dbl

#endif
