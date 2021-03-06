import os
import signal
import random
import time
import errno
import getpass
import shutil
import threading
from subprocess import Popen, PIPE

from . management import Manager


class Server(Manager):
	def __init__(self, **kwargs):
		super().__init__()
		self._server_process = None
		self._dns_proxy_port = random.randint(64500, 64550)
		self._address = kwargs.pop("address", "127.0.0.1")
		self._service_port = kwargs.pop(
			"service_port", random.randint(64555, 64580)
		)
		self._server_pid = None
		self._pidfile = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-test-%s.pid" % self._instance_id
		)
		self._server_params = kwargs.pop("params", {})
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
			"dnsblocker-%s" % self._instance_id
		)

		self._stop_threads_flag = False

	def tail_logfile(self):
		with open(self._logfile, "r") as handle:
			while not self._stop_threads_flag:
				data = handle.read()
				if data:
					self._logger.info(data.rstrip())

	def _setup_env(self):
		#self._logger.info("Setting env")
		os.makedirs(self._dns_proxy_config_destdir)
		with open(self._logfile, "w+") as lh:
			lh.write("# Test case log\n")

	def _cleanup(self):
		shutil.rmtree(self._dns_proxy_config_destdir)
		try:
			os.unlink(self._pidfile)
		except OSError as e:
			if e.errno != errno.ENOENT:
				self._logger.error(e)
				print(e)

	def get_pid(self):
		return self._server_pid

	def get_pidfile_path(self):
		return self._pidfile

	def get_address(self):
		return self._address

	def get_port(self):
		return self._service_port

	def get_dns_port(self):
		return self._dns_proxy_port

	def __enter__(self):
		self._setup_env()
		if self._verbose:
			self._tail_thread = threading.Thread(target=self.tail_logfile)
			self._tail_thread.start()

		executable = os.path.join(
			self._project_root, "service", "bin", "dnsblocker")

		cmd = [
			executable,
			"-D",
			"-v",
			#"-f",
			#"--no-chdir",
			#"--no-close-fds",
			"--no-system-dns-proxy",
			"--no-update",
			"--pidfile", self._pidfile,
			"--logfile", self._logfile,
			"--logger-config-path", self._log_config_path,
			"--network-ip4address", self.get_address(),
			"--dns-proxy", "unbound",
			"--dns-proxy-generate-config",
			"--dns-proxy-port", str(self._dns_proxy_port),
			"--dns-proxy-config-destdir", self._dns_proxy_config_destdir,
			"--service-user", getpass.getuser(),
			"--service-port", str(self._service_port),
			"--db", self._db,
			"--templates-dir", self._templates_dir,
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
		#self._logger.debug("Waiting for pidfile")
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
		#self._logger.debug("### Server: Server ready")
		return self

	def __exit__(self, tp, value, tb):
		with open(self._pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 500
			try:
				#self._logger.debug("### Stopping server %d" % pid)
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
					self._logger.exception(e)
					raise

		if self._verbose:
			self._stop_threads_flag = True
			self._tail_thread.join()
		self._cleanup()
