#ifndef DBL_SERVICE_UNIX_HXX
#define DBL_SERVICE_UNIX_HXX

#include "base.hxx"

#include <string>
#include <memory>
#include <sys/types.h>

namespace dbl {

class UnixService : public BaseService
{
public:
	UnixService() = delete;
	explicit UnixService(std::shared_ptr<RTApi> api);
	virtual ~UnixService() = default;

	virtual void configure();
	virtual void start();
	virtual void stop();

	virtual bool is_already_running();

protected:
	virtual void run_network_discovery();
	virtual bool setup_interface();

	virtual std::string get_default_interface() const;

	virtual void start_dns_proxy();
	virtual void stop_dns_proxy();
	virtual void flush_dns();

	virtual void start_service();
	virtual void save_pidfile();
	virtual void remove_pidfile();

	gid_t group_id_;
	uid_t user_id_;

	pid_t service_pid_;
};

} // dbl
#endif

