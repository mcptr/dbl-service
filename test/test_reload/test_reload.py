from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest
import random
import time


class TestReload(unittest.TestCase):
	def test_reload(self):
		port = random.randint(65000, 65535)
		with Server(service_port=port) as server:
			first_pid = server.get_pid()
			self.assertTrue(first_pid > 0, "Server alive")
			client = Client(server=server)
			response = client.call("status")
			t1 = response.data()["status"]["start_time"]
			self.assertGreater(t1, 0, "Has start timestamp")
			# NOTE: This is naive and bad, but we need
			# to make server spend some time alive
			time.sleep(1)
			response = client.call("reload")
			self.assertTrue(response.is_ok())
			with open(server.get_pidfile_path(), "r") as f:
				second_pid = int(f.read())
				self.assertTrue(second_pid > 0, "Server alive")
				loop = 30
				client = None
				while loop and not client:
					loop -= 1
					time.sleep(0.1)
					try:
						client = Client(
							address=server.get_address(),
							port=port
						)
					except:
						client = None
				self.assertTrue(client is not None, "Got client")
				response = client.call("status")
				t2 = response.data()["status"]["start_time"]
				self.assertGreater(t2, t1, "Reloaded")
