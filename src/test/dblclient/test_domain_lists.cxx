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

	std::string test_domain_name = "test-domain.com";
	dblclient::types::DomainList_t new_list;
	new_list.name = "test-domain-list";
	new_list.url = "http://example/test-domain-list";
	new_list.description = "test domain list";
	new_list.add_domain(test_domain_name);

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

			dblclient::types::DomainListsSet_t lst;
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

	unit_test.test_case(
		"get_domain_list by name",
		[&address, &port, &new_list, &test_domain_name](TestCase& test)
		{
			Session session;
			session.open(address, port);

			dblclient::types::DomainList_t lst;
			lst.name = new_list.name;
			bool ok = session.get_domain_list(lst);
			test.assert_true(ok, "get_domain_list");
			test.assert_equal(lst.name, new_list.name, "name");
			test.assert_equal(lst.url, new_list.url, "url");
			test.assert_equal(
				lst.description,
				new_list.description,
				"description"
			);

			test.assert_equal(
				lst.domains.size(),
				std::size_t(1),
				"size"
			);

			test.assert_equal(
				lst.domains.at(0).name,
				test_domain_name,
				"domain name"
			);
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
