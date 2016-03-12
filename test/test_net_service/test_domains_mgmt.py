from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest


class TestManageDomains(unittest.TestCase):
	def test_block_unblock(self):
		with Server() as server:
			client = Client(server=server)
			for cmd in ["block", "unblock"]:
				response = client.call(cmd, {
					"domains" : "test-domain.com"
				})
				self.assertFalse(
					response.is_ok(), "Not an array / " + cmd
				)

				response = client.call(cmd, {
					"domains" : ["test-domain.com", "test-example.com"]
				})
				self.assertTrue(response.is_ok(), "OK / " + cmd)
