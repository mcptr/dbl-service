from pydbl.test.management import Manager
import unittest
import sqlite3

class TestListsMgmt(unittest.TestCase):
	def test_add_list(self):
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
		
		db = sqlite3.connect(manager.get_db())
		cur = db.cursor()
		cur.row_factory = sqlite3.Row

		st = cur.execute(
			"SELECT * from domain_lists where name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["name"], list_name)
		self.assertEqual(record["url"], list_url)
		self.assertEqual(record["description"], list_description)
