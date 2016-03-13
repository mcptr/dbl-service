#ifndef DBLCLIENT_CLIENT_HXX
#define DBLCLIENT_CLIENT_HXX

#include "session.hxx"
#include <memory>

namespace dblclient {

extern "C" {
	dblclient::Session* create(const std::string& address = "127.0.0.1",
							  unsigned short port = 7654);

	void destroy(dblclient::Session* ptr);
}


} // dblclient

#endif
