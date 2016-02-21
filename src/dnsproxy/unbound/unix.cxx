#include "unix.hxx"
#include "core/common.hxx"
#include "sys/command.hxx"

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


namespace dbl {

UnixUnbound::UnixUnbound(std::shared_ptr<RTApi> api)
	: Unbound(api)
{
	pidfile_path_ = api_->config.dns_proxy_pidfile;
}

std::string UnixUnbound::get_executable_name() const
{
	return "unbound";
}

int UnixUnbound::get_service_pid() const
{
	std::string cmd_output;
	int status = run_command("pidof unbound", cmd_output);

	if(status == 0) {
		boost::trim(cmd_output);
		return std::atoi(cmd_output.c_str());
	}

	return -1;
}

void UnixUnbound::start()
{
	if(!api_->config.no_system_dns_proxy) {
		int current_pid = this->get_service_pid();
		system_proxy_was_running_ = (current_pid > 0);
		LOG(DEBUG) << "Current unbound pid: " << current_pid;

		bool success = this->run_rc("restart");

		if(!success) {
			throw std::runtime_error(
				"Unable to start 'unbound' system service"
			);
		}

		int new_pid = this->get_service_pid();
		int wait_time_ms = 100;
		int wait_counter = 10000 / wait_time_ms;

		while(new_pid < 1 && wait_counter) {
			LOG(INFO) << "Waiting for service: " << wait_counter;
			new_pid = this->get_service_pid();
			std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
			wait_counter--;
		}

		LOG(DEBUG) << "New unbound pid: " << new_pid;

		if(new_pid == current_pid) {
			std::string msg(
				"\nSome init scripts report success even\n"
				"when the service fails to start.\n"
				"You may need to restart 'unbound' service yourself to be sure.\n"
			);
			LOG(INFO) << msg << std::endl;
		}
	}
	else {
		std::string cmd = this->find_executable();
		if(cmd.empty()) {
			throw std::runtime_error("Unable to find dns proxy executable");
		}

		cmd.append(" -v ");
		cmd.append(" -c " + config_file_path_);
		LOG(DEBUG) << cmd << std::endl;
		LOG(DEBUG) << "DNS Proxy config file: " << config_file_path_ << std::endl;
		if(system(cmd.c_str()) != 0) {
			throw std::runtime_error("Unable to start dns proxy. Command: " + cmd);
		}
	}
}

void UnixUnbound::stop()
{
	if(api_->config.dns_proxy_generate_config) {
		LOG(INFO) << "Removing generated configuration file: "
				  << config_file_path_;
		if(unlink(config_file_path_.c_str()) != 0) {
			LOG(ERROR) << "FAILED to remove auto generatated configuration: "
					   << config_file_path_;
		}
	}

	LOG(DEBUG) << "Stopping 'unbound' service";

	if(system_proxy_was_running_) {
		this->run_rc("restart");
	}
	else {
		this->run_rc("stop");
	}

}

bool UnixUnbound::run_rc(const std::string& action) const
{
	std::vector<std::string> cmds = {
		"service unbound " + action,
		"/etc/init.d/unbound " + action,
		"/etc/rc.d/unbound " + action,
		"/usr/local/etc/rc.d/unbound " + action,
		"systemctl " + action + " unbound",
	};

	bool success = false;
	for(auto const& cmd : cmds) {
		LOG(DEBUG) << "Trying: " << cmd;
		if(system(cmd.c_str()) == 0) {
			success = true;
			break;
		}
	}

	return success;
}

} // dbl
