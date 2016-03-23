#include "utils/test.hxx"
#include <string>
#include <memory>
#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace test;
	using dblclient::Session;

	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server());
	std::string address = server->get_address();
	int port = server->get_port();

	std::vector<std::string> block = {
		"block1.example.com",
		"block2.example.com"
	};

	std::vector<std::string> unblock = {
		"unblock1.example.com",
		"unblock2.example.com"
	};

	std::string single_domain = "single.example.net";
		
	unit_test.test_case(
		"block",
		[&address, &port, &block, &single_domain](TestCase& test) {
			Session session;
			session.open(address, port);

			bool ok = session.block_domain(single_domain);
			test.assert_true(ok, "block single");

			ok = session.manage_domains(block, true);
			test.assert_true(ok, "block multiple");

			dblclient::types::DomainSet_t domains;
			ok = session.get_blocked_domains(domains);
			std::cout << session.get_last_error() << std::endl;
			test.assert_true(ok, "get_blocked_domains");
		}
	);

	unit_test.test_case(
		"unblock",
		[&address, &port, &unblock, &single_domain](TestCase& test) {
			Session session;
			session.open(address, port);

			bool ok = session.unblock_domain(single_domain);
			test.assert_true(ok, "unblock single");

			ok = session.manage_domains(unblock, false);
			test.assert_true(ok, "unblock multiple");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
