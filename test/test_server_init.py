from pydbl.test.server import Server
import unittest


class TestServer(unittest.TestCase):
	def test_init(self):
		with Server(verbose=True, preserve_logfile=True) as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")
