#include "session.hxx"
#include "net/service_request.hxx"
#include "dbl/util/crypto.hxx"
#include <iostream>

namespace dblclient {

Session::Session()
	: connection_(new net::ServiceConnection())
{
}

void Session::open(const std::string& address, int port)
{
	connection_->open(address, port);
}

std::string Session::get_raw_data(const std::string& cmd) const
{
	net::ServiceRequest req(cmd);
	auto response = connection_->execute(req);
	return response->get_data().asString();
}

std::string Session::get_server_version() const
{
	net::ServiceRequest req("get_version");
	auto response = connection_->execute(req);
	if(response->is_ok()) {
		return response->get_data()["version"].asString();
	}

	return "";
}

bool Session::authenticate(const std::string& passwd)
{
	net::ServiceRequest req("get_token");
	auto response = connection_->execute(req);
	if(response->is_ok()) {
		std::string token = response->get_data()["token"].asString();
		if(!token.empty()) {
			std::string hash = dbl::crypto::md5(
				dbl::crypto::md5(passwd) + token
			);
			net::ServiceRequest auth_request("auth");
			auth_request.set_parameter("hash", hash);
			response = std::move(connection_->execute(auth_request));
			return response->is_ok();
		}
	}

	return false;
}

bool Session::set_service_password(const std::string& passwd)
{
	net::ServiceRequest req("get_token");
	auto response = connection_->execute(req);
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
			return passwd_response->is_ok();
		}
	}

	return false;
}

bool Session::get_domain_lists(types::DomainListsSet_t& lst) const
{
	lst.clear();
	net::ServiceRequest req("get_domain_lists");
	auto response = connection_->execute(req);
	if(response->is_ok()) {
		for(auto const& domain : response->get_data()["domain_lists"]) {
			
		}
	}
	return lst.size();
}

bool Session::get_blocked_domains(types::DomainSet_t& lst) const
{
	lst.clear();

	return lst.size();
}

} // dblclient
