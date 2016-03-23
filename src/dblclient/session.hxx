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
	Session(const Session&) = delete;
	~Session() = default;

	void open(const std::string& address = "127.0.0.1", int port = 7654);


	bool authenticate(const std::string& passwd);
	bool set_service_password(const std::string& passwd);

	std::string get_raw_data(const std::string& cmd) const;
	std::string get_server_version() const;

	bool get_domain_lists(types::DomainListsSet_t& lst) const;
	bool get_blocked_domains(types::DomainSet_t& lst) const;

private:
	std::unique_ptr<net::ServiceConnection> connection_;
};

} // dblclient

#endif
