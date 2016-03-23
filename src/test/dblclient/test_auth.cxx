#include "utils/test.hxx"
#include <string>
#include <memory>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace test;
	using dblclient::Session;

	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server());
	std::string address = server->get_address();
	int port = server->get_port();

	unit_test.test_case(
		"Test auth (no service password)",
		[&address, port](TestCase& test)
		{
			Session session;
			session.open(address, port);
			bool auth_ok = session.authenticate("");
			test.assert_true(auth_ok, "Auth");
		}
	);

	unit_test.test_case(
		"Test auth (with service password)",
		[&address, port](TestCase& test)
		{
			Session session;
			session.open(address, port);

			std::string password = "test123";
			bool ok = session.set_service_password(password);
			test.assert_true(ok, "Service password set");
			
			bool auth_ok = session.authenticate("invalid");
			test.assert_false(auth_ok, "Invalid auth");

			auth_ok = session.authenticate("");
			test.assert_false(auth_ok, "Invalid auth");

			auth_ok = session.authenticate(password);
			test.assert_true(auth_ok, "Authenticate with valid password");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
