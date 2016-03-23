from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest


class TestGetDomains(unittest.TestCase):
	def test_invalid_type(self):
		with Server() as server:
			client = Client(server=server)

			response = client.call("get_domains", {
				"type": "invalid"
			})
			self.assertFalse(response.is_ok(), "Invalid")


	def test_blocked(self):
		with Server() as server:
			client = Client(server=server)
			blocked_domains = ["testdomain.com", "example-test.com"]
			response = client.call("block", {
				"domains" : blocked_domains
			})
			self.assertTrue(response.is_ok(), "Block domains")

			response = client.call("get_domains", {
				"type": "blocked"
			})
			self.assertTrue(response.is_ok(), "Got blocked domains")
			names = map(lambda d: d["name"], response.data()["domains"])
			for domain in blocked_domains:
				self.assertTrue(domain in names)

	def test_whitelisted(self):
		with Server() as server:
			client = Client(server=server)
			domains = ["testdomain.com", "example-test.com"]
			response = client.call("unblock", {
				"domains" : domains
			})
			self.assertTrue(response.is_ok(), "Unblock domains")

			response = client.call("get_domains", {
				"type": "whitelisted"
			})

			self.assertTrue(response.is_ok(), "Got whitelisted domains")
			names = list(map(lambda d: d["name"], response.data()["domains"]))
			for domain in domains:
				self.assertTrue(domain in names, "Whitelisted: " + domain)

	def test_by_list_id(self):
		with Server() as server:
			client = Client(server=server)
			response = client.call("get_domains", {"list_id": 1})
			self.assertTrue(response.is_ok(), "Got domains")

	def test_get_domain(self):
		with Server() as server:
			client = Client(server=server)
			blocked_domains = ["testdomain.com", "example-test.com"]
			response = client.call("block", {
				"domains" : blocked_domains
			})
			response = client.call("get_domain", {"name": "example-test.com"})
			self.assertTrue(response.is_ok(), "Got domain")
			self.assertEqual(
				response.data()["name"],
				"example-test.com",
				"Name"
			)
