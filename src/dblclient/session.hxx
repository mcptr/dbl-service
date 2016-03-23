#ifndef DBLCLIENT_SESSION_HXX
#define DBLCLIENT_SESSION_HXX

#include "types.hxx"
#include "net/service_connection.hxx"
#include "net/service_response.hxx"
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
	const std::string& get_last_error() const;

	bool authenticate(
		const std::string& passwd);

	bool set_service_password(const std::string& passwd);

	std::string get_raw_data(const std::string& cmd);

	std::string get_server_version();

	bool import_domain_list(const types::DomainList_t& lst);

	bool get_domain_list(types::DomainList_t& lst);
	bool get_domain_lists(types::DomainListsSet_t& lst);

	bool get_blocked_domains(types::DomainSet_t& lst);

private:
	std::unique_ptr<net::ServiceConnection> connection_;
	std::string error_;

	void set_error(const net::ServiceResponse& response);
};

} // dblclient

#endif
