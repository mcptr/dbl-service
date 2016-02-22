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
	gid_t group_id_;
	uid_t user_id_;

	std::string pidof_bin_;

	pid_t service_pid_;
	std::string dns_proxy_pidfile_path_;
	bool system_proxy_was_running_ = false;

	virtual void run_network_discovery();
	virtual bool setup_interface();
	virtual void configure_interface();

	virtual std::string get_default_interface() const;

	virtual void configure_dns_proxy();

	virtual void start_dns_proxy();
	virtual void stop_dns_proxy();
	virtual void flush_dns();

	virtual void start_service();
	virtual void save_pidfile();
	virtual void remove_pidfile();

	virtual std::string find_proxy_executable() const final;
	virtual int get_pid_of(const std::string& program) const;
	virtual bool run_rc(const std::string& action) const;
	virtual std::string get_proxy_executable_name() const;

};

} // dbl
#endif
