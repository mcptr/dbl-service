from pydbl.test.server import Server
from pydbl.test.client import Client
import unittest
import hashlib


class TestAuth(unittest.TestCase):
	def test_cmds_fail_without_auth(self):
		with Server() as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")
			client = Client(server=server)
			hashed_password = hashlib.md5(
				"testpassword".encode("utf-8")).hexdigest()
			response = client.call("get_token")
			token = response.data()["token"]
			concatenated = str(hashed_password + token).encode("utf-8")
			hashed_token = hashlib.md5(concatenated).hexdigest()
			response = client.call("set_service_password", dict(
				password_hash=hashed_password,
				token_hash=hashed_token,
			))

			self.assertTrue(response.is_ok(), "Password changed")

			cmds = [
				"status",
				"set_service_password",
				"remove_service_password",
				"block",
				"unblock",
				"flush_dns",
				"import",
				"delete_list",
				"export_list",
				"reload",
			]

			client = Client(server=server)
			for cmd in cmds:
				response = client.call(cmd)
				self.assertFalse(response.is_ok(), "Denied - not authenticated")

	def test_auth_full(self):
		with Server() as server:
			self.assertTrue(server.get_pid() > 0, "Server alive")

			hashed_password = hashlib.md5(
				"testpassword".encode("utf-8")).hexdigest()

			client = Client(server=server)

			# check status without auth
			response = client.call("status")
			self.assertTrue(response.is_ok(), "Got status without auth")

			# set password
			response = client.call("get_token")
			token = response.data()["token"]
			concatenated = str(hashed_password + token).encode("utf-8")
			hashed_token = hashlib.md5(concatenated).hexdigest()
			response = client.call("set_service_password", dict(
				password_hash=hashed_password,
				token_hash=hashed_token,
			))
			self.assertTrue(response.is_ok(), "Password changed")

			# other client
			second_client = Client(server=server)
			response = second_client.call("status")
			self.assertFalse(response.is_ok(), "Denied status without auth")

			# auth with invalid password
			invalid_password = hashlib.md5(
				"invalid".encode("utf-8")).hexdigest()
			invalid_concatenated = str(invalid_password + token).encode("utf-8")
			invalid_hashed_token = hashlib.md5(
				invalid_concatenated).hexdigest()
			response = second_client.call("auth", {"hash": invalid_hashed_token})
			self.assertFalse(response.is_ok(), "Invalid password")
			self.assertEqual(response.error_message(), "Invalid auth")

			# auth with old token
			response = second_client.call("auth", {"hash": hashed_token})
			self.assertFalse(response.is_ok(), "Invalid password")
			self.assertEqual(response.error_message(), "Invalid auth")

			# auth on first client
			response = client.call("get_token")
			token = response.data()["token"]
			hashed_token = hashlib.md5(
				str(hashed_password + token).encode("utf-8")).hexdigest()

			response = client.call("auth", {"hash": hashed_token})
			self.assertTrue(response.is_ok(), "Auth ok")

			# auth on second client
			response = second_client.call("get_token")
			second_client_token = response.data()["token"]
			self.assertNotEqual(
				token, second_client_token, "Connections yield unique tokens"
			)
			second_hashed_token = hashlib.md5(
				str(hashed_password + second_client_token).encode("utf-8")).hexdigest()

			response = second_client.call("auth", {"hash": second_hashed_token})
			self.assertTrue(response.is_ok(), "Auth ok (client 2)")

			# remove password
			response = client.call("remove_service_password")
			self.assertTrue(response.is_ok(), "Password removed")

			# third client
			second_client = Client(server=server)
			response = second_client.call("status")
			self.assertTrue(response.is_ok(), "Granted after removing password")
