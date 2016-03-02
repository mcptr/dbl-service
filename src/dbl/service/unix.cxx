#include "unix.hxx"
#include "dbl/core/common.hxx"
#include "dbl/util/fs.hxx"
#include "dbl/sys/command.hxx"

#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <csignal>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>


namespace dbl {
namespace service {

Unix::Unix(std::shared_ptr<core::Api> api)
	: Service(api)
{
	pidof_bin_ = dbl::util::fs::find_executable("pidof");
	if(pidof_bin_.empty()) {
		LOG(ERROR) << "Unable to locate 'pidof'. "
				   << "You may need to install it.";
	}
}


bool Unix::is_already_running()
{
	namespace fs = boost::filesystem;
	if(fs::exists(api_->config.service_pidfile)) {
		pid_t pid;
		std::ifstream ifh(api_->config.service_pidfile);
		ifh >> pid;
		ifh.close();

		return (kill(pid, 0) == 0);
	}
	return false;
}

void Unix::run()
{
	if(!api_->config.is_foreground) {
		bool nochdir = api_->config.no_chdir;
		bool noclose = api_->config.no_close_fds;
		if(daemon(nochdir, noclose) != 0) {
			LOG(ERROR) << "Daemonizing failed " << strerror(errno);
			throw std::runtime_error(strerror(errno));
		}

		this->save_pidfile();

		pid_t pid = fork();
		if(pid < 0) {
			LOG(ERROR) << strerror(errno);
			throw std::runtime_error(strerror(errno));
			return;
		}
		else if(pid == 0) {
			signal(SIGTERM, [](int /*sig*/) {
					LOG(INFO) << "Service instance caught SIGTERM";
					Service::service_ptr->stop_service();
				}
			);

			this->run_service();
			_exit(0);
		}
		else {
			this->service_pid_ = pid;

			try {
				this->start_dns_proxy();
				this->flush_dns();
			}
			catch(const std::runtime_error& e) {
				LOG(ERROR) << e.what();
				Service::service_ptr->stop_service();
			}

			signal(SIGTERM, [](int /*sig*/) {
					LOG(INFO) << "Caught SIGTERM. Stopping service";
					// stop() instead of stop_service().
					// kills child, which does stop_service()
					Service::service_ptr->stop();
				}
			);

			int status;
			pid_t chd = wait(&status);

			if(chd == -1) {
				LOG(ERROR) << strerror(errno);
				throw std::runtime_error(strerror(errno));
			}
			else {
				if(WIFSIGNALED(status)) {
					LOG(INFO) << "Service killed by: " << WTERMSIG(status);
				}

				if(WIFEXITED(status)) {
					LOG(INFO) << "Service exited: " << WEXITSTATUS(status);
				}

				this->stop_dns_proxy();
				this->remove_pidfile();
				LOG(INFO) << "################################################";
				LOG(INFO) << "Stopped." << std::endl;
				_exit(0);
			}
		}
	}
	else {
		LOG(INFO) << "Starting foreground instance";
		this->start_dns_proxy();
		this->flush_dns();

		signal(SIGINT, [](int /*sig*/) {
				LOG(DEBUG) << "Foreground instance caught SIGINT";
				Service::service_ptr->stop_service();
			}
		);

		signal(SIGTERM, [](int /*sig*/) {
				LOG(DEBUG) << "Foreground instance caught SIGTERM";
				Service::service_ptr->stop_service();
			}
		);

		this->run_service();
		this->stop_dns_proxy();
	}
}

void Unix::drop_privileges()
{
		struct passwd* p = getpwnam(api_->config.service_user.c_str());

		if(p == nullptr) {
			throw std::runtime_error(
				"Invalid user: " + api_->config.service_user
			);
		}

		uid_t user_id = p->pw_uid;
		gid_t group_id = p->pw_gid;

		if(user_id == 0 || group_id == 0) {
			throw std::runtime_error(
				"Not an unprivileged user: " +
				api_->config.service_user
			);
		}

		if(setgroups(0, nullptr) != 0) {
			PLOG(ERROR) << "setgroups()";
		}
		
		if(setgid(group_id) != 0) {
			PLOG(ERROR) << "setgid(" << group_id << ")";
			throw std::runtime_error(
				"Cannot drop privileges (setgid() failed)"
			);
		}

		if(setegid(group_id) != 0) {
			PLOG(ERROR) << "setegid(" << group_id << ")";
			throw std::runtime_error(
				"Cannot drop privileges (setegid() failed)"
			);
		}

		if(setuid(user_id) != 0) {
			PLOG(ERROR) << "setuid(" << user_id << ")";
			throw std::runtime_error(
				"Cannot drop privileges (setuid() failed)"
			);
		}

		if(seteuid(user_id) != 0) {
			PLOG(ERROR) << "seteuid(" << user_id << ")";
			throw std::runtime_error(
				"Cannot drop privileges (seteuid() failed)"
			);
		}

		if(setuid(0) == 0) {
			throw std::runtime_error("Dropping privileges failed");
		}

		if(setgid(0) == 0) {
			throw std::runtime_error("Dropping privileges failed");
		}
}

void Unix::stop()
{
	if(kill(service_pid_, SIGTERM) < 0) {
		LOG(ERROR) << "Unable to kill service. " << strerror(errno);
	}
}

// void Unix::reload()
// {
// 	LOG(INFO) << "NOT IMPLREMENTED: RELOAD";
// }



void Unix::save_pidfile()
{
	namespace fs = boost::filesystem;
	LOG(INFO) << "Writing pid file: "
			  << api_->config.service_pidfile;

	fs::path path = api_->config.service_pidfile;
	if(!fs::is_directory(path.parent_path())) {
		LOG(DEBUG) << "Creating pidfile directory: "
				   << path.parent_path().string();
		fs::create_directories(path.parent_path());
	}

	if(fs::exists(api_->config.service_pidfile)) {
		unlink(api_->config.service_pidfile.c_str());
	}

	std::ofstream ofh(api_->config.service_pidfile);
	if(!ofh.good()) {
		LOG(ERROR) << "Unable to write pid file";
		throw std::runtime_error("Unable to write pid file.");
	}
	ofh << getpid();
	ofh.close();
}

void Unix::remove_pidfile()
{
	unlink(api_->config.service_pidfile.c_str());
}


void Unix::start_dns_proxy()
{
	LOG(INFO) << "Starting dns proxy";
	if(!api_->config.no_system_dns_proxy) {
		std::string proxy_bin(
			configurator_->get_proxy_executable_name()
		);

		pid_t current_pid = this->get_pid_of(proxy_bin);

		system_proxy_was_running_ = (current_pid > 0);
		LOG(DEBUG) << "Current system proxy pid: " << current_pid;

		bool success = this->run_rc("restart");

		if(!success) {
			throw std::runtime_error(
				"Unable to start system dns proxy service"
			);
		}

		pid_t new_pid = this->get_pid_of(proxy_bin);
		int wait_time_ms = 100;
		int wait_counter = 3000 / wait_time_ms;

		if(new_pid < 1) {
			LOG(INFO) << "Waiting for service...";
			while(new_pid < 1 && wait_counter) {
				new_pid = this->get_pid_of(proxy_bin);

				std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
				wait_counter--;
			}
		}

		//if(new_pid > 0) {
		LOG(DEBUG) << "New dns proxy pid: " << new_pid;
		//}

		if(new_pid == current_pid) {
			std::string msg(
				"\nFAILED to start dnsproxy service\n"
				"\nSome init scripts report success even\n"
				"when the service fails to start.\n"
			);
			msg.append("You may need to restart " 
					   + api_->config.dns_proxy
					   + " service yourself.\n\n"
			);
			msg.append(
				"You should also make sure you don't have other dns software\n"
				"already running."
			);

			LOG(ERROR) << msg << std::endl;
			throw std::runtime_error(
				"Unable to start service: " + api_->config.dns_proxy
			);
		}
	}
	else {
		std::string cmd = configurator_->find_proxy_executable();
		if(cmd.empty()) {
			throw std::runtime_error("Unable to find dns proxy executable");
		}

		cmd.append(" -v ");
		cmd.append(" -c " + dns_proxy_->get_config_path());
		LOG(DEBUG) << cmd << std::endl;
		LOG(DEBUG) << "DNS Proxy config file: " 
				   << dns_proxy_->get_config_path()
				   << std::endl;
		
		std::string output;
		if(sys::run_command(cmd, output) != 0) {
			throw std::runtime_error("Unable to start dns proxy. Command: " + cmd);
		}
	}
}

void Unix::stop_dns_proxy()
{
	LOG(DEBUG) << "Stopping dns proxy";
	if(api_->config.dns_proxy_generate_config) {
		LOG(INFO) << "Removing generated configuration file: "
				  << dns_proxy_->get_config_path();
		if(unlink(dns_proxy_->get_config_path().c_str()) != 0) {
			LOG(ERROR) << "FAILED to remove auto generatated configuration: "
					   << dns_proxy_->get_config_path();
		}
	}

	if(system_proxy_was_running_) {
		LOG(DEBUG) << "Restarting service: " << api_->config.dns_proxy;
		this->run_rc("restart");
	}
	else {
		LOG(DEBUG) << "Stopping service: " << api_->config.dns_proxy;
		this->run_rc("stop");
	}
}

void Unix::flush_dns()
{
}

int Unix::get_pid_of(const std::string& program) const
{
	std::string cmd_output;
	int status = sys::run_command(pidof_bin_ + " " + program, cmd_output);

	if(status == 0) {
		boost::trim(cmd_output);
		return std::atoi(cmd_output.c_str());
	}

	return -1;
}

bool Unix::run_rc(const std::string& action) const
{
	const std::string& proxy = api_->config.dns_proxy;
	std::vector<std::string> cmds = {
		"/usr/sbin/service " + proxy + " " + action,
		"/etc/init.d/" + proxy + " " + action,
		"/etc/rc.d/" + proxy + " " + action,
		"/usr/local/etc/rc.d/" + proxy + " " + action,
		"/usr/sbin/systemctl " + action + " " + proxy,
	};

	bool success = false;
	for(auto const& cmd : cmds) {
		LOG(DEBUG) << "Trying: " << cmd;
		std::string output;
		if(sys::run_command(cmd, output) == 0) {
			LOG(INFO) << "OK, succeeded";
			success = true;
			break;
		}
	}
	return success;
}

} // service
} // dbl