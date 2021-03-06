#include "session.hxx"
#include "net/service_request.hxx"
#include "dbl/util/crypto.hxx"
#include <iostream>

namespace dblclient {

Session::Session()
{
}

void Session::open(const std::string& address, int port)
{
	connection_.reset(new net::ServiceConnection());
	connection_->open(address, port);
}

const std::string& Session::get_last_error() const
{
	return error_;
}

void Session::set_error(const net::ServiceResponse& response)
{
	error_ = response.get_error_msg();
}

std::string Session::get_raw_data(const std::string& cmd)
{
	error_.clear();
	net::ServiceRequest req(cmd);
	auto response = connection_->execute(req);
	set_error(*response);
	return response->get_data().asString();
}

std::string Session::get_server_version()
{
	net::ServiceRequest req("get_version");
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		return response->get_data().asString();
	}

	return "";
}

bool Session::authenticate(const std::string& passwd)
{
	net::ServiceRequest req("get_token");
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		std::string token = response->get_data()["token"].asString();
		if(!token.empty()) {
			std::string hash = dbl::crypto::md5(
				dbl::crypto::md5(passwd) + token
			);
			net::ServiceRequest auth_request("auth");
			auth_request.set_parameter("hash", hash);
			response = std::move(connection_->execute(auth_request));
			set_error(*response);
			return response->is_ok();
		}
	}

	return false;
}

bool Session::set_service_password(const std::string& passwd)
{
	net::ServiceRequest req("get_token");
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		std::string token = response->get_data()["token"].asString();
		if(!token.empty()) {
			std::string passwd_hash = dbl::crypto::md5(passwd);
			std::string hashed_token = dbl::crypto::md5(
				passwd_hash + token
			);

			net::ServiceRequest passwd_request("set_service_password");
			passwd_request.set_parameter("password_hash", passwd_hash);
			passwd_request.set_parameter("token_hash", hashed_token);
			auto passwd_response = connection_->execute(passwd_request);
			set_error(*passwd_response);
			return passwd_response->is_ok();
		}
	}

	return false;
}

bool Session::import_domain_list(const types::DomainList_t& lst)
{
	net::ServiceRequest req("import_domain_list");
	req.set_parameter("domain_list", lst);
	auto response = connection_->execute(req);
	set_error(*response);
	return response->is_ok();
}

bool Session::get_domain_lists(types::DomainListsSet_t& lst)
{
	net::ServiceRequest req("get_domain_lists");
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		for(auto const& d : response->get_data()) {
			types::DomainList_t dl;
			dl.init_from_json(d);
			lst.push_back(dl);
		}

		return true;
	}
	return false;
}

bool Session::get_domain_list(types::DomainList_t& lst)
{
	net::ServiceRequest req("get_domain_list");
	req.set_parameter("name", lst.name);
	req.set_parameter("with_domains", true);
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		lst.init_from_json(response->get_data());
		return true;
	}
	return false;
}

bool Session::get_domain(types::Domain_t& domain)
{
	net::ServiceRequest req("get_domain");
	req.set_parameter("name", domain.name);
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		domain.init_from_json(response->get_data());
		return true;
	}

	return false;
}

bool Session::get_blocked_domains(types::DomainSet_t& lst)
{
	return get_domains(lst, true);
}

bool Session::get_whitelisted_domains(types::DomainSet_t& lst)
{
	return get_domains(lst, false);
}

bool Session::get_domains(types::DomainSet_t& lst, bool blocked)
{
	net::ServiceRequest req("get_domains");
	req.set_parameter("type", (blocked ? "blocked" : "whitelisted"));
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		for(auto const& d : response->get_data()) {
			types::Domain_t domain;
			domain.init_from_json(d);
			lst.push_back(domain);
		}
		return true;
	}

	return false;
}

bool Session::block_domain(const std::string& name)
{
	types::Names_t names;
	names.push_back(name);
	return manage_domains(names, true);
}

bool Session::unblock_domain(const std::string& name)
{
	types::Names_t names;
	names.push_back(name);
	return manage_domains(names, false);
}

bool Session::manage_domains(const types::Names_t& names, bool block)
{
	net::ServiceRequest req((block ? "block" : "unblock"));
	Json::Value arr = Json::arrayValue;

	for(auto const& n : names) {
		arr.append(n);
	}

	req.set_parameter("domains", arr);
	auto response = connection_->execute(req);
	set_error(*response);
	return response->is_ok();
}

void Session::send_reload()
{
	net::ServiceRequest req("reload");
	connection_->send_one_way(req);
}

bool Session::get_status(dbl::status::Status& status)
{
	net::ServiceRequest req("status");
	auto response = connection_->execute(req);
	set_error(*response);
	if(response->is_ok()) {
		status.init_from_json(response->get_data());
	}

	return response->is_ok();
}

} // dblclient
