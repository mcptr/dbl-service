#include "session.hxx"

namespace dblclient {

Session::Session(const std::string& address,
				 unsigned short port)
	: address_(address),
	  port_(port)
{
}

bool Session::open()
{
	return false;
}

bool Session::close()
{
	return false;
}

bool Session::authenticate()
{
	return false;
}

bool Session::get_domain_lists(types::DomainListsSet_t& lst) const
{
	lst.clear();

	return lst.size();
}

bool Session::get_blocked_domains(types::DomainSet_t& lst) const
{
	lst.clear();

	return lst.size();
}

} // dblclient
