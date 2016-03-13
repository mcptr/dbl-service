#include "session.hxx"

namespace dblclient {

Session::Session(const std::string& address,
				 unsigned short port)
	: address_(address),
	  port_(port)
{
}

bool Session::connect()
{
	return false;
}

} // dblclient
