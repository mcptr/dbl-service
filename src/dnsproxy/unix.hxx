#ifndef DBL_DNSPROXY_UNIX_HXX
#define DBL_DNSPROXY_UNIX_HXX

#include "base.hxx"

#include <string>
#include <memory>


namespace dbl {

class Unix
{
public:
	Unix() = delete;
	Unix(std::shared_ptr<RTApi> api);
	virtual ~Unix() = default;

	virtual std::string find_proxy_executable() const final;
	virtual std::string get_executable_name() const = 0;
	virtual void start() = 0;
	virtual void stop() = 0;

protected:
	std::string pidof_;
	std::string pidfile_path_;
	bool system_proxy_was_running_ = false;

	virtual int get_service_pid() const;

	virtual bool run_rc(const std::string& action) const;

};

} // dbl

#endif
