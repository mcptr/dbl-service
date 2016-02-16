#ifndef DBL_SERVICE_UNIX_HXX
#define DBL_SERVICE_UNIX_HXX

#include "base.hxx"

#include <string>
#include <memory>

namespace dbl {

class UnixService : public BaseService
{
public:
	UnixService() = delete;
	explicit UnixService(std::shared_ptr<RTApi> api);
	virtual ~UnixService() = default;
protected:
	virtual void run_network_discovery();
	virtual bool setup_interface();

	virtual std::string get_default_interface() const;

	virtual void start_dns_proxy();
	virtual void stop_dns_proxy();
	virtual void start_http_responder();
	virtual void flush_dns();

};

} // dbl
#endif

