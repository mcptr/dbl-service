#include "service_connection.hxx"
#include "dbl/types/types.hxx"
#include "dbl/manager/domain_manager.hxx"
#include "dbl/manager/domain_list_manager.hxx"

// for static instance - reload
#include "dbl/service/service.hxx"

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
	// if password is not set - treat this connection as authenticated
	is_authenticated_ = auth_.auth("");
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
		if(!e.get_errors().empty()) {
			response_json["error_details"] = Json::arrayValue;

			for(auto const& err : e.get_errors()) {
				response_json["error_details"].append(err);
			}
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
		LOG(DEBUG) << "AUTH HASH" << data["hash"].asString();
		if(!auth_.auth(data["hash"].asString())) {
			throw ServiceOperationError("Invalid auth");
		}
		else {
			is_authenticated_ = true;
		}
	}
	else if(!is_authenticated_) {
		throw ServiceOperationError("Not authenticated");
	}
	else {

		if(cmd.compare("status") == 0) {
			handle_status(data, response_json, errors);
		}
		else if(cmd.compare("set_service_password") == 0) {
			handle_set_service_password(data, response_json, errors);
		}
		else if(cmd.compare("remove_service_password") == 0) {
			handle_remove_service_password(data, response_json, errors);
		}
		else if(cmd.compare("flush_dns") == 0) {
			handle_flush_dns(data, response_json, errors);
		}
		else if(cmd.compare("import") == 0) {
			handle_import(data, response_json, errors);
		}
		else if(cmd.compare("block") == 0) {
			handle_block(data, response_json, errors);
		}
		else if(cmd.compare("unblock") == 0) {
			handle_unblock(data, response_json, errors);
		}
		else if(cmd.compare("get_lists") == 0) {
			handle_get_lists(data, response_json, errors);
		}
		else if(cmd.compare("delete_list") == 0) {
			handle_delete_list(data, response_json, errors);
		}
		else if(cmd.compare("get_domain") == 0) {
			handle_get_domain(data, response_json, errors);
		}
		else if(cmd.compare("get_domains") == 0) {
			handle_get_domains(data, response_json, errors);
		}
		else if(cmd.compare("reload") == 0) {
			handle_reload(data, response_json, errors);
		}
		else {
			throw std::runtime_error("Unknown command");
		}
	}
}


void ServiceConnection::handle_status(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const

{
	response["status"] = "alive";
}

void ServiceConnection::handle_flush_dns(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const

{
	throw ServiceOperationError("not implemented");
}

void ServiceConnection::handle_import(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const

{
	throw ServiceOperationError("not implemented");
}

void ServiceConnection::handle_block(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	manager::DomainManager mgr(api_);
	if(!data["domains"].isArray()) {
		throw ServiceOperationError("Expected array of domains");
	}
	types::Names_t domains;
	for(auto const& domain : data["domains"]) {
		domains.push_back(domain.asString());
	}

	mgr.block_domains(domains);
}

void ServiceConnection::handle_unblock(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	manager::DomainManager mgr(api_);
	if(!data["domains"].isArray()) {
		throw ServiceOperationError("Expected array of domains");
	}
	types::Names_t domains;
	for(auto const& domain : data["domains"]) {
		domains.push_back(domain.asString());
	}

	mgr.unblock_domains(domains);
}

void ServiceConnection::handle_set_service_password(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors)
{
	bool success = auth_.set_password(
		data["password_hash"].asString(),
		data["token_hash"].asString(),
		errors
	);
	if(!success) {
		throw ServiceOperationError("Could not set password", errors);
	}
}

void ServiceConnection::handle_remove_service_password(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors)
{
	auth_.remove_password();
}

void ServiceConnection::handle_get_lists(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	manager::DomainListManager mgr(api_);
	auto result_ptr = mgr.get();
	response["domain_lists"] = Json::arrayValue;
	for(auto const& lst : *result_ptr) {
		response["domain_lists"].append((Json::Value)lst);
	}
}

void ServiceConnection::handle_get_domain(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const
{
	std::string name = data["name"].asString();
	if(name.empty()) {
		throw ServiceOperationError("No domain name given");
	}

	manager::DomainManager mgr(api_);
	auto ptr = mgr.get(name);
	response["domain"] = *ptr;
}

void ServiceConnection::handle_get_domains(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	manager::DomainManager mgr(api_);
	std::string type = data["type"].asString();
	std::unique_ptr<types::DomainSet_t> domains;
	if(type.compare("blocked") == 0) {
		domains = std::move(mgr.get_blocked());
	}
	else if(type.compare("whitelisted") == 0) {
		domains = std::move(mgr.get_whitelisted());
	}
	else {
		int list_id = data["list_id"].asInt();
		if(list_id) {
			domains = std::move(mgr.get(list_id));
		}
	}

	if(!domains) {
		throw ServiceOperationError("Unable to get domains");
	}

	response["domains"] = Json::arrayValue;

	for(auto const& domain : *domains) {
		response["domains"].append(domain);
	}
}

void ServiceConnection::handle_delete_list(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	throw ServiceOperationError("not implemented");
}

void ServiceConnection::handle_reload(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& errors) const
{
	Service::service_ptr->signal_reload();
}

} // server
} // service
} // dbl
