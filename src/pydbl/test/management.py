import os
import time
import logging
import subprocess

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

	def get_base_cmd(self):
		cmd = [
			self._executable,
			"-v",
			"--db", self._db,
			"--logfile", self._logfile,
			"--logger-config-path", self._log_config_path,
		]
		return cmd

	def get_db(self):
		return self._db

	def run(self, options):
		cmd = self.get_base_cmd()
		cmd.extend(options)
		cmd = " ".join(cmd)
		status = os.system(cmd)
		if status:
			self._logger.error(cmd)
		return status

	def read_cmd_output(self, options):
		cmd = self.get_base_cmd()
		cmd.extend(options)
		cmd = " ".join(cmd)
		output = subprocess.check_output(cmd, shell=True)
		return output.decode("utf-8")

	def __del__(self):
		if self._kwargs.get("keep_db", False):
			self._logger.warning("Keeping db: " + self._db)
		else:
			try:
				os.unlink(self._db)
			except OSError as e:
				self._logger.error(e)

		if self._kwargs.get("keep_logfile", False):
			self._logger.warning("Keeping logfile: " + self._logfile)
		else:
			try:
				os.unlink(self._logfile)
			except OSError as e:
				self._logger.error(e)
