#include "utils/test.hxx"
#include <string>
#include <memory>

int main(int argc, char** argv)
{
	using namespace test;
	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server());

	unit_test.test_case(
		"Test version",
		[](TestCase& test)
		{
			test.assert_true(true == true, "");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
