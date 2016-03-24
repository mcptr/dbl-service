from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest


class TestDomainLists(unittest.TestCase):
	def test_all(self):
		with Server() as server:
			client = Client(server=server)
			response = client.call("get_domain_lists")
			self.assertTrue(response.is_ok(), "Got response")
			lst = response.data()
			self.assertEqual(len(lst), 1, "One domain list")
			self.assertEqual(lst[0]["name"], "CUSTOM")
