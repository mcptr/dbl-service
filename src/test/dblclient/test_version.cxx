#include "utils/test.hxx"
#include <string>
#include <memory>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace test;
	using dblclient::Session;

	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server(true));
	std::cout << "PORT: " << server->get_port();

	Session session(server->get_address(), server->get_port());
	unit_test.test_case(
		"Test version",
		[&session](TestCase& test)
		{
			session.open();
			LOG(INFO) << "RAW" << session.get_raw_data("get_version");
			std::string version = session.get_server_version();
			test.assert_true(version.length() > 0, "Got version");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
