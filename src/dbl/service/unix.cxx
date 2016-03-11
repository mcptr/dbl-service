#include "unix.hxx"
#include "worker/unix.hxx"
#include "dbl/core/common.hxx"
#include "dbl/util/fs.hxx"
#include "dbl/sys/command.hxx"

#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <csignal>
#include <cstdlib>
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
	if(api_->config.is_foreground) {
		this->process_foreground();
	}
	else {
		LOG(DEBUG) << "Daemonizing (unix)";
		shm_ptr_.reset(new ipc::SharedMemory<ServiceSHM>("DNSBlocker"));

		bool nochdir = api_->config.no_chdir;
		bool noclose = api_->config.no_close_fds;

		daemon(nochdir, noclose);
		pid_t pid = fork();
		if(pid < 0) {
			PLOG(ERROR) << "daemon()";
			throw std::runtime_error("Daemonizing failed");
		}

		if(pid == 0) {
			this->process_worker();
		}
		else {
			worker_pid_ = pid;
			this->process_master();
		}
	}
}

void Unix::stop()
{
	if(kill(worker_pid_, SIGTERM) < 0) {
		LOG(ERROR) << "Unable to kill service. " << strerror(errno);
	}
}


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

		if(new_pid > 0) {
			LOG(DEBUG) << "New dns proxy pid: " << new_pid;
		}

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

			LOG(ERROR) << msg;
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
		LOG(DEBUG) << cmd;
		LOG(DEBUG) << "DNS Proxy config file: " 
				   << dns_proxy_->get_config_path();
		
		std::string output;
		if(system(cmd.c_str()) != 0) {
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
		if(system(cmd.c_str()) == 0) {
			LOG(DEBUG) << "Succeeded: " << cmd;
			success = true;
			break;
		}
	}
	return success;
}

void Unix::setup_master_signals()
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = [](int sig, siginfo_t* /*siginfo*/, void* /*context*/) {
		switch(sig) {
		case SIGTERM:
		case SIGINT:
			Service::signaled_exit = true;
			LOG(DEBUG) << "Parent caught SIGNAL:" << sig;
			Service::service_ptr->stop();
			break;
		case SIGHUP:
			LOG(DEBUG) << "Parent caught SIGNAL:" << sig;
			Service::service_ptr->stop();
			break;
		case SIGCHLD:
			Service::signaled_exit = true;
			LOG(DEBUG) << "Parent caught SIGCHLD";
			break;
		default:
			LOG(INFO) << "Unhandled signal caught:" << sig;
		}
	};

	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	sigaction(SIGTERM, &act, nullptr);
	sigaction(SIGINT, &act, nullptr);
	sigaction(SIGHUP, &act, nullptr);
	sigaction(SIGCHLD, &act, nullptr);
}

void Unix::run_worker()
{
	worker_ptr_.reset(new worker::Unix(api_));
	worker_ptr_->run(cv_);
}

void Unix::process_worker()
{
	using namespace boost::posix_time;

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = [](int sig, siginfo_t* /*siginfo*/, void* /*context*/) {
		LOG(DEBUG) << "Child caught SIGNAL: " << sig;
		Service::service_ptr->signal_stop();
	};
	act.sa_flags = SA_SIGINFO;

	sigemptyset(&act.sa_mask);
	sigaction(SIGTERM, &act, nullptr);
	sigaction(SIGINT, &act, nullptr);
	sigaction(SIGHUP, &act, nullptr);

	ptime t = microsec_clock::universal_time();
	LOG(DEBUG) << "Waiting for master process";
	auto shm = shm_ptr_->get_object();
	if(shm->sync_semaphore.timed_wait(t + seconds(10))) {
		LOG(DEBUG) << "Starting worker";
		this->run_worker();
	}
	else {
		LOG(ERROR) << "Master process not ready";
	}

	LOG(DEBUG) << "Worker exiting";
	_exit(0);
}

void Unix::process_master()
{
	this->save_pidfile();

	try {
		this->start_dns_proxy();
		this->flush_dns();

		this->setup_master_signals();
		LOG(DEBUG) << "Parent: posting sync semaphore";
		auto shm = shm_ptr_->get_object();
		shm->sync_semaphore.post();
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
		Service::service_ptr->stop();
	}

	int status;
	waitpid(worker_pid_, &status, 0);

	if(WIFSIGNALED(status)) {
		LOG(DEBUG) << "Service worker signaled with:" << WTERMSIG(status);
	}
	else if(WIFEXITED(status)) {
		LOG(DEBUG) <<  "Service worker exit status:" << WEXITSTATUS(status);
	}

	this->stop_dns_proxy();
	this->remove_pidfile();
	LOG(DEBUG) << "Master process finished";
}

void Unix::process_foreground()
{
	this->start_dns_proxy();
	this->flush_dns();
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = [](int sig, siginfo_t* /*siginfo*/, void* /*context*/) {
		Service::signaled_exit = true;
		Service::service_ptr->signal_stop();
	};
	act.sa_flags = SA_SIGINFO;

	sigemptyset(&act.sa_mask);
	sigaction(SIGTERM, &act, nullptr);
	sigaction(SIGINT, &act, nullptr);
	sigaction(SIGHUP, &act, nullptr);

	this->run_worker();
	this->stop_dns_proxy();
	this->flush_dns();
}

} // service
} // dbl
