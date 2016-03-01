#ifndef DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX
#define DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX

#include "connection.hxx"
#include "auth/auth.hxx"
#include "types/types.hxx"

#include <memory>

namespace dbl {
namespace service {
namespace server {

class ServiceOperationError : public std::runtime_error
{
public:
	ServiceOperationError() = default;
	explicit ServiceOperationError(const std::string& msg)
		: std::runtime_error(msg)
	{
	}

	explicit ServiceOperationError(
		const std::string& msg,
		const types::Errors_t& errors)
		: std::runtime_error(msg),
		  errors_(errors)
	{
	}

	~ServiceOperationError() = default;

	virtual const char* what() const throw();

	virtual inline const types::Errors_t& get_errors() const
	{
		return errors_;
	}

protected:
	const std::string msg_;
	const types::Errors_t errors_;
};


class ServiceConnection : public Connection
{
public:
	ServiceConnection() = delete;
	ServiceConnection(std::shared_ptr<core::Api> api,
					  boost::asio::ip::tcp::socket socket);
	virtual ~ServiceConnection() = default;

protected:
	auth::Auth auth_;
	bool is_authenticated_ = false;

	virtual void process_request(const std::string& request,
								 std::string& response);

	virtual void dispatch(const std::string& cmd,
						  const Json::Value& data,
						  Json::Value& value);

};

} // server
} // service
} // dbl

#endif
