#include "unix.hxx"
#include "core/common.hxx"

#include <fstream>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <pwd.h>
#include <csignal>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <boost/filesystem.hpp>


namespace dbl {

UnixService::UnixService(std::shared_ptr<RTApi> api)
	: BaseService(api)
{
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
			signal(SIGTERM, [](int /*sig*/) {
					_exit(0);
				}
			);

			this->start_service();
		}
		else {
			this->service_pid_ = pid;

			this->start_dns_proxy();
			this->flush_dns();

			signal(SIGTERM, [](int /*sig*/) {
					BaseService::service_ptr->stop();
				}
			);

			int status;
			pid_t chd = wait(&status);
			LOG(DEBUG) << "AFTER WAIT EXIT: " << chd;
			if(chd == -1) {
				perror("waitpid");
				LOG(ERROR) << "wait() failed";
				throw std::runtime_error("Child exited abnormally.");
			}
			else {
				if(WIFEXITED(status)) {
					LOG(INFO) << "Exited: " << WEXITSTATUS(status);
				}
				else if(WIFSIGNALED(status)) {
					LOG(INFO) << "Service killed by: " << WTERMSIG(status);
				}

				LOG(INFO) << "Stopping instance: ";
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
	if(api_->config.is_foreground) {
		LOG(WARNING) << "#############################################"
					 << "# WARNING: Running in foreground"
					 << "# without dropping privileges"
					 << "#############################################";
	}
	else {
		LOG(INFO) << "Dropping privileges";
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

		// std::thread srv_thread(
		// 	[this]() {
		// 		this->http_responder_ptr_.reset(
		// 			new service::Server(this->api_->config.service_port)
		// 		);
		// 		server_ptr_->run();
		// 	}
		// );

	}
	//if(api_->config.service_port) {
	this->server_ptr_.reset(
		new service::Server(this->api_->config.service_port)
	);
	LOG(DEBUG) << "INVOKING RUN";
	server_ptr_->run();
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

void UnixService::start_dns_proxy()
{
	LOG(INFO) << "Starting dns proxy";
	dns_proxy_->start();
}

void UnixService::stop_dns_proxy()
{
	LOG(DEBUG) << "Stopping dns proxy";
	dns_proxy_->stop();
}

void UnixService::flush_dns()
{
}

} // dbl
