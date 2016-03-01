#ifndef DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX

#include "connection.hxx"

#include <string>
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
	virtual void process_request(const std::string& request,
								 std::string& response);
};

} // server
} // service
} // dbl

#endif
