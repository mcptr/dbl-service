#ifndef DBLCLIENT_SESSION_HXX
#define DBLCLIENT_SESSION_HXX

#include <string>

namespace dblclient {

class Session
{
public:
	Session() = default;
	Session(const std::string& address = "127.0.0.1",
			unsigned short port = 7654);
	Session(const Session&) = delete;
	~Session() = default;

	bool connect();
private:
	const std::string address_ = "127.0.0.1";
	const unsigned short port_ = 7654;
};

} // dblclient

#endif
