#include "http_responder_connection.hxx"

#include <string>
#include <vector>


namespace dbl {
namespace service {


HTTPResponderConnection::HTTPResponderConnection(
	std::shared_ptr<dbl::RTApi> api,
	boost::asio::ip::tcp::socket socket)
	: Connection(api, std::move(socket))
{
}

bool HTTPResponderConnection::process_request()
{
	response_.clear();

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
		response_.append(item + "\r\n");
	}

	response_.append("\r\n");

	return true;
}

} // service
} // dbl

