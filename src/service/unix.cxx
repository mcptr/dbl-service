#include "unix.hxx"
#include "core/common.hxx"
#include "util/fs.hxx"
#include "sys/command.hxx"

#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <csignal>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


namespace dbl {

UnixService::UnixService(std::shared_ptr<RTApi> api)
	: BaseService(api)
{
	dns_proxy_pidfile_path_ = api_->config.dns_proxy_pidfile;

	pidof_bin_ = dbl::find_executable("pidof");
	if(pidof_bin_.empty()) {
		LOG(ERROR) << "Unable to locate 'pidof'. "
				   << "You may need to install it.";
	}
}

void UnixService::configure()
{
	struct passwd* p = getpwnam(api_->config.service_user.c_str());

	if(p == nullptr) {
		throw std::runtime_error(
			"Invalid user: " + api_->config.service_user
		);
	}

	user_id_ = p->pw_uid;
	group_id_ = p->pw_gid;

	if(user_id_ == 0 || group_id_ == 0) {
		throw std::runtime_error(
			"Not an unprivileged user: " +
			api_->config.service_user
		);
	}

	this->BaseService::configure();
}

bool UnixService::is_already_running()
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

void UnixService::save_pidfile()
{
	namespace fs = boost::filesystem;
	LOG(INFO) << "Writing pid file: "
			  << api_->config.service_pidfile;
	if(fs::exists(api_->config.service_pidfile)) {
		unlink(api_->config.service_pidfile.c_str());
	}

	std::ofstream ofh(api_->config.service_pidfile);
	if(!ofh.good()) {
		throw std::runtime_error("Unable to write pid file.");
	}
	ofh << getpid();
	ofh.close();
}

void UnixService::remove_pidfile()
{
	unlink(api_->config.service_pidfile.c_str());
}


void UnixService::start()
{
	if(!api_->config.is_foreground) {
		if(daemon(0, 0) != 0) {
			perror("daemon()");
			throw std::runtime_error("Daemonizing failed.");
		}

		this->save_pidfile();

		pid_t pid = fork();
		if(pid < 0) {
			perror("fork()");
			LOG(ERROR) << "fork() failed";
			return;
		}
		else if(pid == 0) {
			BaseService::service_mtx_.lock();
			signal(SIGTERM, [](int /*sig*/) {
					BaseService::service_mtx_.unlock();
				}
			);

			this->start_service();
		}
		else {
			this->service_pid_ = pid;

			try {
				this->start_dns_proxy();
				this->flush_dns();
			}
			catch(const std::runtime_error& e) {
				LOG(ERROR) << e.what();
				BaseService::service_ptr->stop();
			}

			signal(SIGTERM, [](int /*sig*/) {
					BaseService::service_ptr->stop();
				}
			);

			int status;
			pid_t chd = wait(&status);

			if(chd == -1) {
				perror("waitpid");
				LOG(ERROR) << "wait() failed";
				throw std::runtime_error("Child exited abnormally.");
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
				_exit(0);
			}
		}
	}
	else {
		LOG(INFO) << "Starting foreground instance";
		signal(SIGINT, [](int /*sig*/) {
				BaseService::service_ptr->stop();
			}
		);

		this->start_dns_proxy();
		this->flush_dns();

		this->start_service();
	}
}

void UnixService::start_service()
{
	this->BaseService::start_service();

	if(api_->config.is_foreground) {
		LOG(WARNING) << "\n#############################################\n"
					 << "# WARNING: Running in foreground\n"
					 << "# without dropping privileges.\n"
					 << "#\n"
					 << "# Use SIGINT to quit\n"
					 << "#############################################\n";

		BaseService::service_mtx_.lock();

		signal(SIGINT, [](int /*sig*/) {
				BaseService::service_mtx_.unlock();
			}
		);

	}
	else {
		LOG(INFO) << "Dropping privileges";
		if(setgroups(0, nullptr) != 0) {
			perror("setgroups()");
			throw std::runtime_error(
				"Cannot drop privileges (setgroups() failed)"
			);
		}

		if(setgid(group_id_) != 0) {
			perror("setgid()");
			throw std::runtime_error(
				"Cannot drop privileges (setgid() failed)"
			);
		}

		if(setuid(user_id_) != 0) {
			perror("setuid()");
			throw std::runtime_error(
				"Cannot drop privileges (setuid() failed)"
			);
		}

		if(setuid(0) == 0) {
			throw std::runtime_error("Dropping privileges failed");
		}

		if(setgid(0) == 0) {
			throw std::runtime_error("Dropping privileges failed");
		}
	}

	this->serve();
	if(api_->config.is_foreground) {
		this->stop_dns_proxy();
	}
	_exit(0);
}

void UnixService::stop()
{
	if(kill(service_pid_, SIGTERM) < 0) {
		LOG(ERROR) << "Unable to kill service, errno: " << errno;
	}
}

std::string UnixService::get_default_interface() const
{
	return "lo";
}

void UnixService::run_network_discovery()
{
	char buf[1024];
	struct ifconf ifc;
	struct ifreq *ifr;
	int sock;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		throw std::runtime_error("run_network_discovery(): socket() failed");
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
		throw std::runtime_error(
			"run_network_discovery(): ioctl(SIOCGIFCONF) failed"
		);
	}

	ifr = ifc.ifc_req;
	int number_of_interfaces  = ifc.ifc_len / sizeof(struct ifreq);
	for(int i = 0; i < number_of_interfaces; i++) {
		struct ifreq *item = &ifr[i];
		struct sockaddr_in* saddr = (struct sockaddr_in*) &item->ifr_addr;
		std::string address(inet_ntoa(saddr->sin_addr));
		available_interfaces_[item->ifr_name].insert(address);
	}
}

bool UnixService::setup_interface()
{
	struct ifreq ifr;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	strncpy(ifr.ifr_name, interface_.c_str(), IFNAMSIZ);

	ifr.ifr_addr.sa_family = AF_INET;
	struct sockaddr_in* saddr = (struct sockaddr_in*) &ifr.ifr_addr;

	inet_pton(AF_INET, ip4address_.c_str(), &saddr->sin_addr);
	if(ioctl(sock, SIOCSIFADDR, &ifr) < 0) {
		perror("ioctl(SIOCSIFADDR)");
	}
	if(ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCGIFFLAGS)");
	}

	ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
	if(ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCGIFFLAGS)");
	}

	return true;
}

void UnixService::configure_interface()
{
	bool ok = this->setup_interface();
	if(!ok) {
		throw std::runtime_error("Interface configuration failed");
	}
}

void UnixService::start_dns_proxy()
{
	LOG(INFO) << "Starting dns proxy";
	if(!api_->config.no_system_dns_proxy) {
		pid_t current_pid = this->get_pid_of(
			this->get_proxy_executable_name()
		);
		system_proxy_was_running_ = (current_pid > 0);
		LOG(DEBUG) << "Current unbound pid: " << current_pid;

		bool success = this->run_rc("restart");

		if(!success) {
			throw std::runtime_error(
				"Unable to start 'unbound' system service"
			);
		}

		pid_t new_pid = this->get_pid_of(
			this->get_proxy_executable_name()
		);
		int wait_time_ms = 100;
		int wait_counter = 3000 / wait_time_ms;

		if(new_pid < 1) {
			LOG(INFO) << "Waiting for service...";
			while(new_pid < 1 && wait_counter) {
				new_pid = this->get_pid_of(
					this->get_proxy_executable_name()
				);

				std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
				wait_counter--;
			}
		}

		//if(new_pid > 0) {
		LOG(DEBUG) << "New unbound pid: " << new_pid;
		//}

		if(new_pid == current_pid) {
			std::string msg(
				"\nFAILED to start dnsproxy service\n"
				"\nSome init scripts report success even\n"
				"when the service fails to start.\n"
				"You may need to restart 'unbound' service yourself.\n\n"
				"You should also make sure you don't have other dns software\n"
				"already running."
			);

			LOG(ERROR) << msg << std::endl;
			throw std::runtime_error("Unable to start 'unbound' service");
		}
	}
	else {
		std::string cmd = this->find_proxy_executable();
		if(cmd.empty()) {
			throw std::runtime_error("Unable to find dns proxy executable");
		}

		cmd.append(" -v ");
		cmd.append(" -c " + dns_proxy_->get_config_path());
		LOG(DEBUG) << cmd << std::endl;
		LOG(DEBUG) << "DNS Proxy config file: " 
				   << dns_proxy_->get_config_path()
				   << std::endl;
		if(system(cmd.c_str()) != 0) {
			throw std::runtime_error("Unable to start dns proxy. Command: " + cmd);
		}
	}
	//dns_proxy_->start();
}

void UnixService::stop_dns_proxy()
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
		LOG(DEBUG) << "Restarting 'unbound' service";
		this->run_rc("restart");
	}
	else {
		LOG(DEBUG) << "Stopping 'unbound' service";
		this->run_rc("stop");
	}
	//dns_proxy_->stop();
}

void UnixService::flush_dns()
{
}

int UnixService::get_pid_of(const std::string& program) const
{
	std::string cmd_output;
	int status = run_command(pidof_bin_ + " " + program, cmd_output);

	if(status == 0) {
		boost::trim(cmd_output);
		return std::atoi(cmd_output.c_str());
	}

	return -1;
}

bool UnixService::run_rc(const std::string& action) const
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
			LOG(INFO) << "OK, succeeded";
			success = true;
			break;
		}
	}
	return success;
}

std::string UnixService::get_proxy_executable_name() const
{
	if(api_->config.dns_proxy.compare("dnsmasq") == 0) {
		return "dnsmasq";
	}

	return "unbound";
}

std::string UnixService::find_proxy_executable() const
{

	std::string executable = api_->config.dns_proxy_executable;

	if(executable.empty()) {
		executable = dbl::find_executable(
			this->get_proxy_executable_name()
		);
	}

	if(executable.empty()) {
		throw std::runtime_error("Unable to locate dns proxy software");
	}

	LOG(INFO) << "DNS proxy program: " << executable;
	return executable;
}

void UnixService::configure_dns_proxy()
{
	dns_proxy_->set_value("PIDFILE", dns_proxy_pidfile_path_);
	this->BaseService::configure_dns_proxy();
}

} // dbl
