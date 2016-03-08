from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest


class TestReload(unittest.TestCase):
	def test_reload(self):
		with Server(verbose=True, keep_logfile=True) as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")
			client = Client(address="127.0.0.1", port=7654)
			response = client.call("reload")
			print(response.data())
