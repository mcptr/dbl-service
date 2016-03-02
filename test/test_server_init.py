from pydbl.test.server import Server
import unittest
import time

class TestServer(unittest.TestCase):
	def test_init(self):
		with Server(verbose=True) as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")
