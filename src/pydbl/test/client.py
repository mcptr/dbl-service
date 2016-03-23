import json
import socket


class Response(object):
	def __init__(self, data):
		self._data = json.loads(data)

	def is_ok(self):
		return (self._data.get("success", False) is True)

	def error_message(self):
		return self._data.get("error_message", "")

	def data(self):
		return self._data.get("data", {})


class Client(object):
	def __init__(self, **kwargs):
		server = kwargs.pop("server", None)
		if server:
			self._address = server.get_address()
			self._port = server.get_port()
		else:
			self._address = kwargs.pop("address")
			self._port = kwargs.pop("port")
		self._conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._conn.connect((self._address, self._port))
		self._conn.setblocking(True)

	def call(self, cmd, data = None):
		request = {
			"command": cmd.lower(),
			"data": (data or {})
		}
		self._send(json.dumps(request))
		resp = self._read()
		return Response(resp)

	def _send(self, data):
		try:
			self._conn.sendall(data.encode("utf-8"))
			return True
		except socket.error as e:
			print("_send(): ", e)
		return False

	def _read(self):
		result = ""
		buf_size = 512
		try:
			while True:
				buf = bytearray(buf_size)
				view = memoryview(buf)
				n = self._conn.recv_into(view, buf_size)
				if n:
					result += view[:n].tobytes().decode("utf-8")
					if n < buf_size:
						break
				else:
					break
		except socket.error as e:
			print("_read(): ", e)
		return result

	def __del__(self):
		try:
			self._conn.shutdown(socket.SHUT_RDWR)
			self._conn.close()
		except OSError as e:
			print(e)
			
