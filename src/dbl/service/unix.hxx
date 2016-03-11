#ifndef DBL_SERVICE_UNIX_HXX
#define DBL_SERVICE_UNIX_HXX

#include "service.hxx"

#include <string>
#include <memory>
#include <sys/types.h>

namespace dbl {
namespace service {

class Unix : public Service
{
public:
	Unix() = delete;
	explicit Unix(std::shared_ptr<core::Api> api);
	virtual ~Unix() = default;

	virtual bool is_already_running();
	virtual void run();
	virtual void stop();

protected:
	pid_t worker_pid_;
	bool system_proxy_was_running_ = false;

	std::string pidof_bin_;

	virtual void setup_signals();
	virtual void start_dns_proxy();
	virtual void stop_dns_proxy();
	virtual void flush_dns();

	virtual void save_pidfile();
	virtual void remove_pidfile();

	virtual void run_worker();

	virtual int get_pid_of(const std::string& program) const;
	virtual bool run_rc(const std::string& action) const;
};

} // service
} // dbl

#endif
