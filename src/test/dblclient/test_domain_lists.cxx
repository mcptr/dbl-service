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

	dbl::types::DomainList new_list;
	new_list.name = "test-domain-list";
	new_list.url = "http://example/test-domain-list";
	new_list.add_domain("test-domain.com");

	unit_test.test_case(
		"import_domain_list",
		[&address, &port, &new_list](TestCase& test)
		{
			Session session;
			session.open(address, port);
			bool ok = session.import_domain_list(new_list);
			test.assert_true(ok, "import_domain_list");
		}
	);

	unit_test.test_case(
		"get_domain_lists",
		[&address, &port, &new_list](TestCase& test)
		{
			Session session;
			session.open(address, port);

			dbl::types::DomainListsSet_t lst;
			bool ok = session.get_domain_lists(lst);
			test.assert_true(ok, "get_domain_lists");
			bool found;
			for(auto const& it : lst) {
				if(it.name.compare(new_list.name) == 0) {
					found = true;
					break;
				}
			}
			test.assert_true(found, "found imported list");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
