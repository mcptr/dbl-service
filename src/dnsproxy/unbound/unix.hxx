#ifndef DBL_DNSPROXY_UNBOUND_UNIX_HXX
#define DBL_DNSPROXY_UNBOUND_UNIX_HXX

#include "unbound.hxx"

namespace dbl {

class UnixUnbound : public Unbound
{
public:
	UnixUnbound() = delete;
	UnixUnbound(std::shared_ptr<RTApi> api);
	virtual ~UnixUnbound() = default;

	virtual std::string get_executable_name() const;
	virtual void start();
	virtual void stop();

protected:
	std::string pidof_;
	virtual int get_service_pid() const;

	virtual bool run_rc(const std::string& action) const;
};

};

#endif
