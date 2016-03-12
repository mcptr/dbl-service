from pydbl.test.management import Manager
import unittest
import sqlite3

class TestListsMgmt(unittest.TestCase):
	def test_list_operations(self):
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
		self.assertEqual(record["active"], 1)

		status = manager.run([
			"--disable-list", list_name,
		])

		st = cur.execute(
			"SELECT * from domain_lists where name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["active"], 0, "List disabled")


		status = manager.run([
			"--enable-list", list_name,
		])

		st = cur.execute(
			"SELECT * from domain_lists where name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["active"], 1, "List enanled")
		status = manager.run([
			"--delete-list", list_name,
		])
		self.assertEqual(status, 0)
		st = cur.execute(
			"SELECT count(*) as cnt from domain_lists where name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["cnt"], 0, "List removed")
