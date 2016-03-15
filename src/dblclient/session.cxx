#include "session.hxx"
#include "net/service_request.hxx"

namespace dblclient {

Session::Session()
	: Session("127.0.0.1", 7654)
{
}

Session::Session(const std::string& address,
				 unsigned short port)
	: connection_(net::ServiceConnection(address, port))
{
}

void Session::open()
{
	connection_.open();
}

bool Session::authenticate()
{
	return false;
}

bool Session::get_domain_lists(types::DomainListsSet_t& lst) const
{
	lst.clear();
	net::ServiceRequest req("get_domain_lists");
	auto response = connection_.execute(req);
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
