#include "service_connection.hxx"
#include "dbl/types/types.hxx"

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <json/json.h>

namespace dbl {
namespace service {
namespace server {

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
			types::Errors_t errors {reader.getFormattedErrorMessages()};
			LOG(ERROR) << reader.getFormattedErrorMessages();
			throw ServiceOperationError("Invalid format", errors);
		}

		std::string cmd(root["command"].asString());
		boost::algorithm::to_lower(cmd);
		Json::Value& data = root["data"];

		response_json["success"] = true;
		this->dispatch(cmd, data, response_json);
	}
	catch(const ServiceOperationError& e) {
		response_json["success"] = false;
		response_json["error_message"] = e.what();
		response_json["error_details"] = Json::arrayValue;

		for(auto const& err : e.get_errors()) {
			response_json["error_details"].append(err);
		}
	}
	catch(const std::runtime_error& e) {
		response_json["success"] = false;
		response_json["error_message"] = e.what();
	}

	response = response_json.toStyledString();
}

void ServiceConnection::dispatch(const std::string& cmd,
								 const Json::Value& data,
								 Json::Value& response_json)
{
	types::Errors_t errors;

	if(cmd.compare("get_token") == 0) {
		response_json["token"] = auth_.get_token();
	}
	else if(cmd.compare("auth") == 0) {
		is_authenticated_ = auth_.auth(
			data["token"].asString(),
			data["hash"].asString()
		);
		response_json["auth_status"] = is_authenticated_;
	}
	else if(!is_authenticated_) {
		response_json["auth_error"] = true;
	}
	else {
		if(cmd.compare("set_service_password") == 0) {
			auth_.set_password(
				data["password_hash"].asString(),
				data["hashed_token"].asString(),
				errors
			);
		}
		if(cmd.compare("remove_service_password") == 0) {
			auth_.remove_password();
		}
		else if(cmd.compare("flush_dns") == 0) {
			throw ServiceOperationError("not implemented");

		}
		else if(cmd.compare("import") == 0) {
			throw ServiceOperationError("not implemented");

		}
		else if(cmd.compare("block") == 0) {
			
			//api_->db()->block_domains(data["domains"].asArray());
		}
		else if(cmd.compare("unblock") == 0) {

		}
		else if(cmd.compare("delete_list") == 0) {

		}
		else if(cmd.compare("export_list") == 0) {

		}
		else if(cmd.compare("reload") == 0) {

		}
		else {
			throw std::runtime_error("Unknown command");
		}
	}
}

} // server
} // service
} // dbl
