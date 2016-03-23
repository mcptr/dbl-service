#include "server.hxx"
#include "common.hxx"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <stdexcept>
#include <random>
#include <fstream>
#include <cstdio>
#include <boost/filesystem.hpp>

namespace test {

Server::Server()
	: Server("127.0.0.1", 7654)
{
}

Server::Server(bool verbose)
	: Server("127.0.0.1", 7654, verbose)
{
}

Server::Server(const std::string& address, int port, bool verbose)
	: TestDaemon(verbose),
	  address_(address),
	  port_(port)
{
	char* pr = getenv("PROJECT_ROOT");
	if(pr == nullptr) {
		throw std::runtime_error("PROJECT_ROOT env var not set");
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<> distribution(50000, 60000);

	port_ = distribution(engine);
	dns_proxy_port_ = distribution(engine);

	project_root_.assign(pr);

	std::string instance_name = (
		"cxx-test-dnsblocker-" + std::to_string(getpid())
	);

	instance_tmp_dir_ = "/tmp/" + instance_name;
	std::string instance_etc_dir = (
		project_root_ + "/service/etc/dnsblocker"
	);

	pidfile_ = instance_tmp_dir_ + "/server.pid";
	logfile_ = instance_tmp_dir_ + "/server.log";

	logger_config_path_ = instance_etc_dir + "/log.conf";
	templates_dir_ = instance_etc_dir + "/templates";

	dns_proxy_config_destdir_ = instance_tmp_dir_;
	db_ = instance_tmp_dir_ + "/service.db";
}

Server::~Server()
{
	cleanup();
}

void Server::set_arguments(std::vector<std::string>& args)
{
	std::string prog = project_root_ + "/service/bin/dnsblocker";
	args.push_back(prog);

	if(is_verbose()) {
		args.push_back("-v");
	}

	if(is_debug()) {
		args.push_back("-D");
	}

	args.push_back("--no-system-dns-proxy");
	args.push_back("--no-update");
	args.push_back("--pidfile=" + pidfile_);
	args.push_back("--logfile=" + logfile_);
	args.push_back("--logger-config-path=" + logger_config_path_);
	args.push_back("--network-ip4address=" + address_);
	args.push_back("--dns-proxy=unbound");
	args.push_back("--dns-proxy-generate-config");
	args.push_back("--dns-proxy-port=" + std::to_string(dns_proxy_port_));
	args.push_back("--dns-proxy-config-destdir=" + dns_proxy_config_destdir_);
	args.push_back("--service-user=" + get_current_user());
	args.push_back("--service-port=" + std::to_string(port_));
	args.push_back("--db=" + db_);
	args.push_back("--templates-dir=" + templates_dir_);
}

const std::string& Server::get_address() const
{
	return address_;
}

int Server::get_port() const
{
	return port_;
}

pid_t Server::get_pid() const
{
	std::ifstream f(pidfile_);
	int pid = 0;
	if(f.good()) {
		f >> pid;
		f.close();
	}
	return pid;
}

bool Server::is_ready() const
{
	bool alive = false;
	size_t wait_loops = 100;
	while(!alive && wait_loops) {
		wait_loops--;
		if(get_pid() > 0) {
			alive = true;
		}
		else {
			usleep(1000 * 50);
		}
	}

	// TODO: This is racy. Check if we're able to connect.
	// Pidfile may be created before the service starts.

	return true;
}

std::string Server::get_current_user() const
{
	struct passwd* pwd = nullptr;
	pwd = getpwuid(geteuid());
	if(pwd != nullptr) {
		return pwd->pw_name;
	}

	return "";
}

void Server::cleanup() const
{
	if(no_cleanup_) {
		LOG(INFO) << "Preserved directory: " << instance_tmp_dir_;
	}
	else {
		boost::filesystem::remove_all(instance_tmp_dir_);
	}
}

} // test
