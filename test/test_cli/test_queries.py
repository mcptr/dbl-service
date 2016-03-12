from pydbl.test.management import Manager
import unittest


class TestCLI(unittest.TestCase):
	def test_lists(self):
		manager = Manager(verbose=True)
		list_name = "test-list"
		list_url = "http://test.example.com/list"
		list_description = "description"
		status = manager.run([
			"-A",
			"--list-name", list_name,
			"--list-url", list_url,
			"--list-description", list_description,
		])
		self.assertEqual(status, 0)

		expected = "test-list :: Active: 1, Custom: 0 ::  description"
		output = manager.read_cmd_output(["-QL"])
		matched_lines = len(list(filter(
			lambda l: l.strip() == expected, output.split("\n"))
				 )
		)
		self.assertEqual(matched_lines, 1, "Found added list")

