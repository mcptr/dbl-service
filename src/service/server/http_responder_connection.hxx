#ifndef DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX
#define DBL_SERVICE_SERVER_HTTP_RESPONDER_CONNECTION_HXX

#include "connection.hxx"

#include <memory>

namespace dbl {
namespace service {

class HTTPResponderConnection : public Connection

{
public:
	HTTPResponderConnection() = delete;
	HTTPResponderConnection(std::shared_ptr<dbl::RTApi> api,
							boost::asio::ip::tcp::socket socket);
	virtual ~HTTPResponderConnection() = default;

protected:
	virtual bool process_request();
};

}
} // dbl

#endif
