#include "utils/test.hxx"
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

bool find_domain(const dblclient::types::DomainSet_t& set,
				 const std::string& name)
{
	dblclient::types::DomainSet_t::const_iterator it = std::find_if(
		set.begin(),
		set.end(),
		[&name](const dblclient::types::Domain_t& d) {
			return (name.compare(d.name) == 0);
		}
	);

	return (it != set.end());
}

int main(int argc, char** argv)
{
	using namespace test;
	using namespace dblclient::types;
	using dblclient::Session;

	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server());
	std::string address = server->get_address();
	int port = server->get_port();

	std::vector<std::string> test_set = {
		"microsoft.com",
		"live.com"
	};

	std::string single_domain = "facebook.com";

	unit_test.test_case(
		"block",
		[&address, &port, &test_set, &single_domain](TestCase& test) {
			Session session;
			session.open(address, port);

			bool ok = session.block_domain(single_domain);
			test.assert_true(ok, "block single");

			ok = session.manage_domains(test_set, true);
			test.assert_true(ok, "block multiple");

			DomainSet_t set;
			ok = session.get_blocked_domains(set);
			test.assert_true(ok, "get_blocked_domains");

			ok = find_domain(set, single_domain);
			test.assert_true(ok, "found single domain");

			for(auto const& item : test_set) {
				ok = find_domain(set, item);
				test.assert_true(ok, "found " + item);
			}
		}
	);

	unit_test.test_case(
		"unblock single",
		[&address, &port, &test_set, &single_domain](TestCase& test) {
			Session session;
			session.open(address, port);

			bool ok = session.unblock_domain(single_domain);
			test.assert_true(ok, "unblock " + single_domain);

			DomainSet_t set;
			ok = session.get_blocked_domains(set);
			test.assert_true(ok, "get_blocked_domains");

			bool found = find_domain(set, single_domain);
			test.assert_false(found, "unblocked");

			for(auto const& item : test_set) {
				ok = find_domain(set, item);
				test.assert_true(ok, "ok, sill blocked " + item);
			}

			DomainSet_t whitelisted;
			ok = session.get_whitelisted_domains(whitelisted);
			test.assert_true(ok, "get_whitelisted_domains");

			found = find_domain(whitelisted, single_domain);
			test.assert_true(found, "whitelisted " + single_domain);
		}
	);

	unit_test.test_case(
		"unblock multiple",
		[&address, &port, &test_set](TestCase& test) {
			Session session;
			session.open(address, port);

			bool ok = session.manage_domains(test_set, false);
			test.assert_true(ok, "unblock multiple");

			DomainSet_t set;
			ok = session.get_blocked_domains(set);
			test.assert_true(ok, "get_blocked_domains");

			DomainSet_t whitelisted;
			ok = session.get_whitelisted_domains(whitelisted);
			test.assert_true(ok, "get_whitelisted_domains");

			for(auto const& item : test_set) {
				bool found = find_domain(set, item);
				test.assert_false(found, "unblocked " + item);
				found = find_domain(whitelisted, item);
				test.assert_true(found, "whitelisted " + item);
			}
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
