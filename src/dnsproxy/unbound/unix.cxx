#include "unix.hxx"
#include "core/common.hxx"

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <boost/filesystem.hpp>


namespace dbl {

UnixUnbound::UnixUnbound(std::shared_ptr<RTApi> api)
	: Unbound(api)
{
	pidfile_path_ = (
		api_->program_options.get<std::string>("dns-proxy-pidfile")
	);
}

std::string UnixUnbound::get_executable_name() const
{
	return "unbound";
}

void UnixUnbound::start()
{
	std::string cmd = this->find_executable();
	if(cmd.empty()) {
		throw std::runtime_error("Unable to find dns proxy executable");
	}

	cmd.append(" -c " + config_file_path_);
	if(system(cmd.c_str()) != 0) {
		throw std::runtime_error("Unable to start dns proxy. Command: " + cmd);
	}
}

} // dbl
