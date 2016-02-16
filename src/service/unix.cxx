#include "unix.hxx"
#include "core/common.hxx"

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>


namespace dbl {

UnixService::UnixService(std::shared_ptr<RTApi> api)
	: BaseService(api)
{
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
}

void UnixService::start_http_responder()
{
}

void UnixService::flush_dns()
{
}

} // dbl
