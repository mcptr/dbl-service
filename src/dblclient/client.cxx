#include "client.hxx"


namespace dbclient {

extern "C" {
	dblclient::Session* create(const std::string& address,
							  unsigned short port)
	{
		return new dblclient::Session(address, port);
	}

	void destroy(dblclient::Session* ptr)
	{
		delete ptr;
	}
}

} // dblclient
