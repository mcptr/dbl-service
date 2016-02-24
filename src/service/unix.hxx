#ifndef DBL_SERVICE_UNIX_HXX
#define DBL_SERVICE_UNIX_HXX

#include "base.hxx"
#include "dnsproxy/base.hxx"

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

	virtual bool is_already_running();
	virtual void run();
	virtual void stop();
	//virtual void start();
	//virtual void reload();


protected:
	pid_t service_pid_;
	bool system_proxy_was_running_ = false;

	std::string pidof_bin_;

	virtual void start_dns_proxy();
	virtual void stop_dns_proxy();
	virtual void flush_dns();

	virtual void drop_privileges();
	virtual void save_pidfile();
	virtual void remove_pidfile();

	virtual int get_pid_of(const std::string& program) const;
	virtual bool run_rc(const std::string& action) const;
};

} // dbl
#endif
