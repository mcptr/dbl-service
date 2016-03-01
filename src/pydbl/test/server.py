import os
import signal
import random
import time
import errno
from subprocess import Popen, PIPE


class Server(object):
	def __init__(self, **kwargs):
		self._server_process = None
		self._project_root = os.getenv("PROJECT_ROOT")
		self._service_port = random.randint(65500, 65535)
		self._pidfile = os.path.join("tmp", "dbl-test-%d.pid" % os.getpid())
		self._verbose = kwargs.pop("verbose")
		self._server_params = kwargs.pop("params", {})
		self._logfile = os.path.join("tmp", "dbl-log-%d.log" % os.getpid())


	def __enter__(self):
		executable = os.path.join(self._project_root, "bin", "dnsblocker")
		cmd = [
			executable, "-D", "-v",
			"--basedir", self._project_root,
			"--pidfile", self._pidfile,
			"--logfile", self._logfile
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

		if not os.path.isfile(self.__pidfile):
			raise Exception("Server startup failed")

		wait = 10
		try:
			with open(self._pidfile, "r") as fh:
				pid = int(fh.read())
				while wait and (os.kill(pid, 0) is not None):
					time.sleep(0.1)
					wait -= 1
				if os.kill(pid, 0) is not None:
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
		except OSError as e:
			if e.errno != errno.ENOENT:
				print(e)
