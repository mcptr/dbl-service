#include "session.hxx"
#include "net/service_request.hxx"
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

bool Session::authenticate()
{
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
