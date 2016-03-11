#include "http_responder_connection.hxx"

#include <string>
#include <vector>


namespace dbl {
namespace service {
namespace server {

HTTPResponderConnection::HTTPResponderConnection(
	std::shared_ptr<core::Api> api,
	boost::asio::ip::tcp::socket socket)
	: Connection(api, std::move(socket))
{
}

void HTTPResponderConnection::process_request(const std::string& /*request*/,
											  std::string& response)
{
	std::vector<std::string> reply;

	reply.push_back(
		"HTTP/1.1 "
		+ std::to_string(this->api_->config.http_responder_status_code)
		+ " "
		+ this->api_->config.http_responder_status_text
	);

	reply.push_back("Server: HTTPResponder");
	reply.push_back("Content-Type: text/html");
	reply.push_back("Content-Length: 0");

	for(auto const& item : reply) {
		response.append(item + "\r\n");
	}

	response.append("\r\n\r\n");
}

} // server
} // service
} // dbl

