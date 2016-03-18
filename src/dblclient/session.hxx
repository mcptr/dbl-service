#ifndef DBLCLIENT_SESSION_HXX
#define DBLCLIENT_SESSION_HXX

#include "types.hxx"
#include "net/service_connection.hxx"
#include <string>
#include <memory>

namespace dblclient {

class Session
{
public:
	Session();
	explicit Session(const std::string& address,
					 unsigned short port);
	Session(const Session&) = delete;
	~Session() = default;

	void open();

	std::string get_raw_data(const std::string& cmd) const;

	bool authenticate();
	std::string get_server_version() const;

	bool get_domain_lists(types::DomainListsSet_t& lst) const;
	bool get_blocked_domains(types::DomainSet_t& lst) const;

private:
	net::ServiceConnection connection_;
};

} // dblclient

#endif
