#include "service_connection.hxx"
#include "dbl/core/constants.hxx"
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
	Json::Value response_data = Json::objectValue;

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

		LOG(DEBUG) << "CMD:" << cmd;
		LOG(DEBUG) << "DATA:" << data;
		response_json["success"] = true;
		this->dispatch(cmd, data, response_data);
		response_json["data"] = response_data;
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
		else if(cmd.compare("block") == 0) {
			handle_block(data, response_json, errors);
		}
		else if(cmd.compare("unblock") == 0) {
			handle_unblock(data, response_json, errors);
		}
		else if(cmd.compare("import_domain_list") == 0) {
			handle_import_domain_list(data, response_json, errors);
		}
		else if(cmd.compare("get_domain_lists") == 0) {
			handle_get_domain_lists(data, response_json, errors);
		}
		else if(cmd.compare("get_domain_list") == 0) {
			handle_get_domain_list(data, response_json, errors);
		}
		else if(cmd.compare("delete_domain_list") == 0) {
			handle_delete_domain_list(data, response_json, errors);
		}
		else if(cmd.compare("get_version") == 0) {
			handle_get_version(data, response_json, errors);
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
			LOG(ERROR) << "Unkown command: " << cmd;
			throw std::runtime_error("Unknown command");
		}
	}
}


void ServiceConnection::handle_status(
	const Json::Value& /* data */,
	Json::Value& response,
	types::Errors_t& /* errors */) const
{
	api_->status.to_json(response);
}

void ServiceConnection::handle_flush_dns(
	const Json::Value& /* data */,
	Json::Value& /* response */,
	types::Errors_t& /* errors */) const
{
	throw ServiceOperationError("not implemented");
}

void ServiceConnection::handle_block(
	const Json::Value& data,
	Json::Value& /*response*/,
	types::Errors_t& /*errors*/) const
{
	manager::DomainManager mgr(api_);
	types::Names_t domains;

	if(!data["domains"].isArray()) {
		throw ServiceOperationError("Domains must be an array");
	}

	for(auto const& domain : data["domains"]) {
		domains.push_back(domain.asString());
	}

	mgr.block_domains(domains);
}

void ServiceConnection::handle_unblock(
	const Json::Value& data,
	Json::Value& /*response*/,
	types::Errors_t& /*errors*/) const
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
	Json::Value& /*response*/,
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
	const Json::Value& /* data */,
	Json::Value& /* response */,
	types::Errors_t& /* errors */)
{
	auth_.remove_password();
}

void ServiceConnection::handle_import_domain_list(
	const Json::Value& data,
	Json::Value& /* response */,
	types::Errors_t& /* errors */) const
{
	if(!data["domain_list"].isObject()) {
		throw ServiceOperationError("Invalid data.");
	}

	types::DomainList lst;
	lst.init_from_json(data["domain_list"]);
	manager::DomainListManager mgr(api_);
	mgr.import(lst, data["custom"].asBool());
}

void ServiceConnection::handle_get_domain_lists(
	const Json::Value& /* data */,
	Json::Value& response,
	types::Errors_t& /* errors */) const
{
	manager::DomainListManager mgr(api_);
	auto result_ptr = mgr.get();
	response = Json::arrayValue;
	for(auto const& lst : *result_ptr) {
		response.append((Json::Value)lst);
	}
}

void ServiceConnection::handle_get_domain_list(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& /* errors */) const
{
	std::string name = data["name"].asString();
	if(name.empty()) {
		throw ServiceOperationError("No name given.");
	}
	manager::DomainListManager mgr(api_);
	auto result_ptr = mgr.get(name, data["with_domains"].asBool());
	result_ptr->to_json(response);
}

void ServiceConnection::handle_get_version(
		const Json::Value& /*data*/,
		Json::Value& response,
		types::Errors_t& /*errors*/) const
{
	response = dbl::core::constants::VERSION;
}

void ServiceConnection::handle_get_domain(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& /*errors*/) const
{
	std::string name = data["name"].asString();
	if(name.empty()) {
		throw ServiceOperationError("No domain name given");
	}

	manager::DomainManager mgr(api_);
	auto ptr = mgr.get(name);
	response = *ptr;
}

void ServiceConnection::handle_get_domains(
	const Json::Value& data,
	Json::Value& response,
	types::Errors_t& /*errors*/) const
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
	else if(data["list_id"].asInt()) {
		int list_id = data["list_id"].asInt();
		if(list_id) {
			domains = std::move(mgr.get(list_id));
		}
	}
	else {
		throw ServiceOperationError("Need type.");
	}

	if(!domains) {
		throw ServiceOperationError("Unable to get domains");
	}

	response = Json::arrayValue;

	for(auto const& domain : *domains) {
		response.append(domain);
	}
}

void ServiceConnection::handle_delete_domain_list(
	const Json::Value& /* data */,
	Json::Value& /*response*/,
	types::Errors_t& /* errors */) const
{
	throw ServiceOperationError("not implemented");
}

void ServiceConnection::handle_reload(
	const Json::Value& /* data */,
	Json::Value& /* response */,
	types::Errors_t& /* errors */)
{
	socket_.cancel();
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close();
	Service::service_ptr->signal_stop();
}

} // server
} // service
} // dbl
