#include "unix.hxx"
#include "util/fs.hxx"

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>


namespace dbl {
namespace service {
namespace configurator {

Unix::Unix(const std::shared_ptr<core::Api> api)
	: Configurator(api)
{
}

std::string Unix::get_default_interface() const
{
	return "lo";
}

void Unix::run_network_discovery()
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

void Unix::configure_interface()
{
	bool failed = false;
	struct ifreq ifr;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	strncpy(ifr.ifr_name, interface_.c_str(), IFNAMSIZ);

	ifr.ifr_addr.sa_family = AF_INET;
	struct sockaddr_in* saddr = (struct sockaddr_in*) &ifr.ifr_addr;

	inet_pton(AF_INET, ip4address_.c_str(), &saddr->sin_addr);
	if(ioctl(sock, SIOCSIFADDR, &ifr) < 0) {
		perror("ioctl(SIOCSIFADDR)");
		failed = true;
	}
	else if(ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCGIFFLAGS)");
		failed = true;
	}

	if(!failed) {
		ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
		if(ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
			perror("ioctl(SIOCGIFFLAGS)");
		}
		failed = true;
	}

	if(failed) {
		throw std::runtime_error("Interface configuration failed");
	}
}

std::string Unix::get_proxy_executable_name() const
{
	if(api_->config.dns_proxy.compare("dnsmasq") == 0) {
		return "dnsmasq";
	}
	else if(api_->config.dns_proxy.compare("unbound") == 0) {
		return "unbound";
	}

	throw std::runtime_error("Invalid dns proxy");
}

std::string Unix::find_proxy_executable() const
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

void Unix::configure_dns_proxy(dnsproxy::DNSProxy& proxy)
{
	proxy.set_value("PIDFILE", api_->config.dns_proxy_pidfile);
	this->Configurator::configure_dns_proxy(proxy);
}

} // configurator
} // service
} // dbl
