import os
import signal
import random
import time
import errno
import getpass
import shutil
import threading
import logging
from subprocess import Popen, PIPE

class Server(object):
	def __init__(self, **kwargs):
		self._kwargs = kwargs
		self._logger = logging.getLogger(__name__)
		self._server_process = None
		self._project_root = os.getenv("PROJECT_ROOT")
		self._virtual_env_root = os.getenv("VIRTUAL_ENV")
		self._dns_proxy_port = random.randint(64500, 64550)
		self._service_port = random.randint(64555, 64580)
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
		self._templates_dir = os.path.join(
			self._project_root,
			"service",
			"etc",
			"dnsblocker",
			"templates"
		)
		self._dns_proxy_config_destdir = os.path.join(
			self._virtual_env_root,
			"var",
			"run",
			"dnsblocker-%d" % os.getpid()
		)

		self._tail_thread = threading.Thread(target=self.tail_logfile)
		self._stop_threads_flag = False

	def _setup_env(self):
		self._logger.info("Setting env")
		os.makedirs(self._dns_proxy_config_destdir)
		with open(self._logfile, "w+") as lh:
			lh.write("# Test case log\n")

	def _cleanup_env(self):
		shutil.rmtree(self._dns_proxy_config_destdir)
		if not self._kwargs.pop("preserve_logfile", False):
			os.unlink(self._logfile)

		try:
			os.unlink(self._pidfile)
		except OSError as e:
			if e.errno != errno.ENOENT:
				self._logger.error(e)
				#print(e)

	def get_pid(self):
		return self._server_pid

	def tail_logfile(self):
		with open(self._logfile, "r") as handle:
			while not self._stop_threads_flag:
				data = handle.read()
				if data:
					self._logger.info(data.rstrip())
					#print(data, end="")

	def __enter__(self):
		self._setup_env()
		self._tail_thread.start()

		executable = os.path.join(
			self._project_root, "service", "bin", "dnsblocker")

		cmd = [
			executable,
			"-D",
			"-v",
			#"-f",
			"--no-system-dns-proxy",
			"--no-update",
			"--no-chdir",
			"--no-close-fds",
			"--dns-proxy-generate-config",
			"--pidfile", self._pidfile,
			"--logfile", self._logfile,
			"--dns-proxy", "unbound",
			"--dns-proxy-port", str(self._dns_proxy_port),
			"--service-user", getpass.getuser(),
			"--service-port", str(self._service_port),
			"--db", self._db,
			"--templates-dir", self._templates_dir,
			"--dns-proxy-config-destdir", self._dns_proxy_config_destdir
		]

		for sparam in self._server_params:
			option = sparam
			if not option.startswith("--"):
				option = "--" + sparam
			cmd.extend([option, str(self._server_params[sparam])])

		if self._verbose:
			self._logger.debug(" ".join(cmd))

		self._server_process = Popen(cmd, stdout=PIPE, shell=False)
		wait = 25
		self._logger.debug("Waiting for pidfile")
		while wait and not os.path.isfile(self._pidfile):
			time.sleep(0.1)
			wait -= 1

		if not os.path.isfile(self._pidfile):
			self._stop_threads_flag = True
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
			self._logger.exception(e)
			self._stop_threads_flag = True
		self._logger.debug("### Server: Server ready")
		return self

	def __exit__(self, tp, value, tb):
		with open(self._pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 500
			try:
				self._logger.debug("### Stopping server %d" % pid)
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
					self._logger.error("Killing with SIGKILL")
					os.kill(pid, signal.SIGKILL)
				self._logger.debug("### Server: Server stopped")
			except OSError as e:
				if e.errno != errno.ESRCH:
					print(")!@&#()*!@&#)(*!@&)()@#")
					self._logger.exception(e)
					raise
		self._stop_threads_flag = True
		self._tail_thread.join()
		self._cleanup_env()
