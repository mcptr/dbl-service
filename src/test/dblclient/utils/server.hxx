#ifndef DBL_TEST_UTIL_SERVER_HXX
#define DBL_TEST_UTIL_SERVER_HXX

#include <string>
#include <cxx-test-util/util.hxx>


namespace test {

class Server : public TestDaemon
{
public:
	Server();
	Server(bool verbose);
	explicit Server(const std::string& address,
					int port = 7654,
					bool verbose = false);

	virtual ~Server();

	void set_arguments(std::vector<std::string>& args);
	bool is_ready() const;
	pid_t get_pid() const;

	const std::string& get_address() const;
	int get_port() const;

protected:
	virtual void cleanup() const;

	std::string get_current_user() const;

	std::string address_ = "127.0.0.1";
	int port_ = 7654;

	std::string project_root_;
	std::string instance_tmp_dir_;

	std::string pidfile_;
	std::string logfile_;
	std::string logger_config_path_;

	std::string dns_proxy_config_destdir_;
	int dns_proxy_port_ = 0;

	std::string service_user_;
	std::string db_;
	std::string templates_dir_;

};


} // test


#endif
