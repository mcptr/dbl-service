from __future__ import absolute_import
from . import base


class Linux(base.Base):
	def get_include_paths(self):
		return [
			"/usr/include",
			"/usr/local/include"
		]

	def get_lib_paths(self):
		return [
			"/usr/lib",
			"/usr/lib64",
			"/usr/local/lib",
			"/usr/local/lib64",
			"/usr/lib/x86_64-linux-gnu"
		]

	def get_libs(self):
		return ["dl"]

	def get_static_libs(self):
		return []

	def get_linkflags(self):
		return ["-lrt"]

	def get_static_linkflags(self):
		return ["-static"]
