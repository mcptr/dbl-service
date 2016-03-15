#ifndef DBLCLIENT_SESSION_HXX
#define DBLCLIENT_SESSION_HXX

#include "types.hxx"
#include <string>
#include <memory>

namespace dblclient {

class Session
{
public:
	Session() = default;
	explicit Session(const std::string& address = "127.0.0.1",
					 unsigned short port = 7654);
	Session(const Session&) = delete;
	~Session() = default;

	bool open();
	bool close();

	bool authenticate();
	bool get_domain_lists(types::DomainListsSet_t& lst) const;
	bool get_blocked_domains(types::DomainSet_t& lst) const;

private:
	const std::string address_ = "127.0.0.1";
	const unsigned short port_ = 7654;

};

} // dblclient

#endif
