import os
import signal
import random
import time
import errno
from subprocess import Popen, PIPE


class Server(object):
	def __init__(self, **kwargs):
		self._kwargs = kwargs
		self._server_process = None
		self._project_root = os.getenv("PROJECT_ROOT")
		self._virtual_env_root = os.getenv("VIRTUAL_ENV")
		self._service_port = random.randint(65500, 65535)
		self._dns_proxy_port = random.randint(64500, 64535)
		self._server_pid = None
		self._pidfile = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-test-%d.pid" % os.getpid()
		)
		self._verbose = kwargs.pop("verbose", False)
		self._server_params = kwargs.pop("params", {})
		self._logfile = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-log-%d.log" % os.getpid()
		)
		self._db = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-db-%d.db" % os.getpid()
		)

	def get_pid(self):
		return self._server_pid

	def __enter__(self):
		executable = os.path.join(
			self._project_root, "service", "bin", "dnsblocker")
		cmd = [
			executable,
			"-D",
			"-N",
			"-v",
			"--pidfile", self._pidfile,
			"--logfile", self._logfile,
			"--dns-proxy", "unbound"
			"--dns-proxy-port", self._dns_proxy_port,
			"--service-user", current,
			"--service-port", self._dns_proxy_port,
			"--no-system-dns-proxy",
			"--no-update",
			"--db", 
			
		]
		for sparam in self._server_params:
			option = sparam
			if not option.startswith("--"):
				option = "--" + sparam
			cmd.extend([option, str(self._server_params[sparam])])

		if self._verbose:
			print(" ".join(cmd))

		self.__server_process = Popen(cmd, stdout=PIPE, shell=False)
		print("")
		wait = 3
		while not os.path.isfile(self._pidfile):
			time.sleep(0.1)
			wait -= 1

		if not os.path.isfile(self._pidfile):
			raise Exception("Server startup failed")

		wait = 10
		try:
			with open(self._pidfile, "r") as fh:
				self._server_pid = int(fh.read())
				while wait and (os.kill(self._server_pid, 0) is not None):
					time.sleep(0.1)
					wait -= 1
				if os.kill(self._server_pid, 0) is not None:
					raise Exception("Server startup failed")
		except Exception as e:
			print(e)
		print("### Server: Server ready")
		return self

	def __exit__(self, tp, value, tb):
		with open(self._pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 500
			try:
				print("### Stopping server", pid)
				os.kill(pid, signal.SIGTERM)
				is_alive = True
				while wait and is_alive:
					try:
						is_alive = (os.kill(pid, 0) is None)
						if is_alive:
							time.sleep(0.02)
							wait -= 1
						else:
							break
					except OSError as e:
						if e.errno == errno.ESRCH:
							break
				if os.kill(pid, 0) is None:
					print("Killing with SIGKILL")
					os.kill(pid, signal.SIGKILL)
				print("### Server: Server stopped")
			except OSError as e:
				if e.errno != errno.ESRCH:
					raise
		try:
			os.unlink(self._pidfile)
			if not self._kwargs.pop("preserve_logfile", False):
				os.unlink(self._logfile)
		except OSError as e:
			if e.errno != errno.ENOENT:
				print(e)
