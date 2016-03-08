from __future__ import absolute_import
from . import base


class Clang(base.Base):
	def __init__(self, options):
		base.Base.__init__(self, "clang", options)

	def get_bin(self):
		return "clang++"

	def get_linkflags(self):
		return ["-pthread"]

	def get_libs(self):
		return []
