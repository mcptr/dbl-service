import os
import time
import logging

stdout_handler = logging.StreamHandler()

class Manager(object):
	def __init__(self, **kwargs):
		self._kwargs = kwargs
		self._verbose = kwargs.pop("verbose", False)
		self._project_root = os.getenv("PROJECT_ROOT")
		self._virtual_env_root = os.getenv("VIRTUAL_ENV")
		self._logger = logging.getLogger(__name__)
		if self._verbose:
			self._logger.addHandler(stdout_handler)
		self._logger.setLevel(logging.DEBUG)

		self._instance_id = time.time()
		self._executable = os.path.join(
			self._project_root, "service", "bin", "dnsblocker"
		)
		self._logfile = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-log-%s.log" % self._instance_id
		)
		self._log_config_path = os.path.join(
			self._project_root,
			"service",
			"etc",
			"dnsblocker",
			"log.conf"
		)
		self._db = os.path.join(
			self._virtual_env_root,
			"tmp",
			"dbl-db-%s.db" % self._instance_id
		)

	def get_db(self):
		return self._db

	def run(self, options):
		cmd = [
			self._executable,
			"--db", self._db,
			"--logfile", self._logfile,
			"--logger-config-path", self._log_config_path,
		]
		cmd.extend(options)
		cmd = " ".join(cmd)
		status = os.system(cmd)
		if status:
			self._logger.error(cmd)
		return status
