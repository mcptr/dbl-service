from __future__ import absolute_import
from . import base


class GCC(base.Base):
	def __init__(self, options):
		base.Base.__init__(self, "gcc", options)

	def get_bin(self):
		return "clang++"
