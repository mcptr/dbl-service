#include "linux.hxx"
#include "core/common.hxx"


namespace dbl {

LinuxService::LinuxService(std::shared_ptr<RTApi> api)
	: UnixService(api)
{
}

void LinuxService::configure_interface()
{
	bool ok = this->setup_interface();
	if(!ok) {
		throw std::runtime_error("Interface configuration failed");
	}
}

} // dbl
