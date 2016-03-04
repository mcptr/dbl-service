from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest


class TestDomainLists(unittest.TestCase):
	def test_all(self):
		with Server() as server:
			client = Client(server=server)
			response = client.call("get_lists")
			self.assertTrue(response.is_ok(), "Got response")
			self.assertTrue("domain_lists" in response.data(), "Body")
			lst = response.data()["domain_lists"]
			self.assertEqual(len(lst), 1, "One domain list")
			self.assertEqual(lst[0]["name"], "CUSTOM")
