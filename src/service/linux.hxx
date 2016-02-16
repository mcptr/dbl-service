#ifndef DBL_SERVICE_LINUX_HXX
#define DBL_SERVICE_LINUX_HXX

#include "unix.hxx"
#include "options/options.hxx"

#include <unordered_map>
#include <string>
#include <memory>

namespace dbl {

class LinuxService : public UnixService
{
public:
	LinuxService() = delete;
	explicit LinuxService(std::shared_ptr<RTApi> api);
	virtual ~LinuxService() = default;

protected:
	virtual void configure_interface();

private:
	bool interface_exists(const std::string& name);
};

} // dbl


#endif
