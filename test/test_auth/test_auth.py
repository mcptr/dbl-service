from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest
import hashlib


class TestAuth(unittest.TestCase):
	def test_init(self):
		with Server() as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")
			client = Client(server=server)
			response = client.call("get_token")
			print(response.data())
			self.assertTrue(response.is_ok(), "Got response")
		
			
