#include "service_connection.hxx"
#include "types/types.hxx"

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <json/json.h>

namespace dbl {
namespace service {
namespace server {

const char* ServiceOperationError::what() const throw()
{
	return msg_.c_str();
}

ServiceConnection::ServiceConnection(
	std::shared_ptr<core::Api> api,
	boost::asio::ip::tcp::socket socket)
	: Connection(api, std::move(socket)),
	  auth_(auth::Auth(api))
{
	// if password is not set - allow all commands
	is_authenticated_ = auth_.auth(auth_.get_token(), "");
}

void ServiceConnection::process_request(const std::string& request,
										std::string& response)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value response_json = Json::objectValue;

	try {
		bool success = reader.parse(request, root, false);
		if(!success) {
			LOG(ERROR) << reader.getFormattedErrorMessages();
			throw ServiceOperationError("Invalid format");
		}

		std::string cmd(root["COMMAND"].asString());
		boost::algorithm::to_upper(cmd);
		Json::Value& data = root["DATA"];

		this->dispatch(cmd, data, response_json);
	}
	catch(const ServiceOperationError& e) {
		response_json["status"] = "ERROR";
		response_json["status_message"] = e.what();
		response_json["error_details"] = Json::arrayValue;

		for(auto const& err : e.get_errors()) {
			response_json["error_details"].append(err);
		}
	}
	catch(const std::runtime_error& e) {
		response_json["status"] = "ERROR";
		response_json["status_message"] = e.what();
	}

	response = response_json.toStyledString();
}

void ServiceConnection::dispatch(const std::string& cmd,
								 const Json::Value& data,
								 Json::Value& response_json)
{
	types::Errors_t errors;

	if(cmd.compare("GET_TOKEN") == 0) {
		response_json["TOKEN"] = auth_.get_token();
	}
	else if(cmd.compare("AUTH") == 0) {
		is_authenticated_ = auth_.auth(
			data["TOKEN"].asString(),
			data["HASH"].asString()
		);
		response_json["AUTH_STATUS"] = is_authenticated_;
	}
	else if(!is_authenticated_) {
		response_json["AUTH_ERROR"] = true;
	}
	else {
		if(cmd.compare("SET_SERVICE_PASSWORD") == 0) {
			auth_.set_password(
				data["password_hash"].asString(),
				data["hashed_token"].asString(),
				errors
			);
		}
		if(cmd.compare("REMOVE_SERVICE_PASSWORD") == 0) {
			auth_.remove_password();
		}
		else if(cmd.compare("FLUSH_DNS") == 0) {
			throw ServiceOperationError("not implemented");

		}
		else if(cmd.compare("IMPORT") == 0) {
			throw ServiceOperationError("not implemented");

		}
		else if(cmd.compare("BLOCK") == 0) {
			
			//api_->db()->block_domains(data["domains"].asArray());
		}
		else if(cmd.compare("UNBLOCK") == 0) {

		}
		else if(cmd.compare("DELETE_LIST") == 0) {

		}
		else if(cmd.compare("EXPORT_LIST") == 0) {

		}
		else if(cmd.compare("RELOAD") == 0) {

		}
	}
}

} // server
} // service
} // dbl
