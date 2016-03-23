#ifndef DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX
#define DBL_SERVICE_SERVER_SERVICE_CONNECTION_HXX

#include "connection.hxx"
#include "dbl/auth/auth.hxx"
#include "dbl/types/types.hxx"

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

	virtual inline const types::Errors_t& get_errors() const
	{
		return errors_;
	}

protected:
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

	void dispatch(const std::string& cmd,
				  const Json::Value& data,
				  Json::Value& value);

	// op handlers
	void handle_status(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;


	void handle_flush_dns(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_block(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_unblock(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_set_service_password(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors);

	void handle_remove_service_password(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors);

	void handle_import_domain_list(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_get_domain_lists(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_get_domain_list(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_delete_domain_list(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_get_version(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_get_domain(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_get_domains(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;

	void handle_reload(
		const Json::Value& data,
		Json::Value& response,
		types::Errors_t& errors) const;
};

} // server
} // service
} // dbl

#endif
