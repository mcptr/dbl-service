#ifndef DBLCLIENT_ERROR_HXX
#define DBLCLIENT_ERROR_HXX

namespace dblclient {

class DBLClientError : public std::runtime_error
{
public:
	DBLClientError()
		: std::runtime_error("Unknown error")
	{
	}

	DBLClientError(const std::string& msg)
		: std::runtime_error(msg)
	{
	}

	virtual ~DBLClientError() = default;
};

} // dblclient

#endif
