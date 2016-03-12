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

		st = cur.execute("SELECT * FROM domain_lists WHERE name = ?", (list_name, ))
		record = st.fetchone()
		self.assertEqual(record["name"], list_name)
		self.assertEqual(record["url"], list_url)
		self.assertEqual(record["description"], list_description)
		self.assertEqual(record["active"], 1, "Created as active")

		# add some domains
		max_domains = 3
		for i in range(0, max_domains):
			cur.execute(
				"INSERT INTO domains(name, list_id) VALUES (?, ?)",
				("example-%d.com" % i, record["id"])
			)
			db.commit()

		cur.execute(
			"SELECT count(*) AS cnt FROM domains d" +
			" JOIN domain_lists AS dl ON dl.id = d.list_id " +
			" WHERE dl.name = ?",
			(list_name, )
		)
		record = cur.fetchone()
		self.assertEqual(record["cnt"], max_domains)

		status = manager.run([
			"--disable-list", list_name
		])

		self.assertEqual(status, 0, "Disable list command ok")

		st = cur.execute(
			"SELECT * FROM domain_lists WHERE name = ?",
			(list_name, )
		)
		record = st.fetchone()
		self.assertEqual(record["active"], 0, "List disabled")


		status = manager.run([
			"--enable-list", list_name
		])
		self.assertEqual(status, 0, "Enable list command ok")

		st = cur.execute(
			"SELECT * FROM domain_lists WHERE name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["active"], 1, "List enanled")
		status = manager.run([
			"--delete-list", list_name
		])
		self.assertEqual(status, 0)
		st = cur.execute(
			"SELECT count(*) AS cnt FROM domain_lists WHERE name = ?",
			(list_name, ))
		record = st.fetchone()
		self.assertEqual(record["cnt"], 0, "List removed")

		cur.execute(
			"SELECT count(*) AS cnt FROM domains d" +
			" JOIN domain_lists dl ON dl.id = d.list_id " +
			" WHERE dl.name = ?",
			(list_name, )
		)
		record = cur.fetchone()
		self.assertEqual(record["cnt"], 0)
