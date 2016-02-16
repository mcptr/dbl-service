from __future__ import absolute_import
import os

from . compiler.clang import Clang
from . compiler.gcc import GCC
from . platform.linux import Linux


PLATFORM = os.uname()[0].lower()


class Builder(object):
	def __init__(self, options, compiler="clang", platform=PLATFORM):
		self._config = dict(
			CXX=None,
			CXXFLAGS=[],
			CPPDEFINES=[],
			CPPPATH=[],
			LIBS=[],
			LIBPATH=[],
			LINKFLAGS=[]
		)
		compiler = compiler.lower()
		platform = platform.lower()

		compilers = {
			"clang": Clang,
			"clang++": Clang,
			"gcc": GCC,
			"g++": GCC,
		}

		platforms = {
			"linux": Linux,
		}

		self._static_build = False

		if compiler not in compilers:
			raise Exception("Unsupported compiler: " + compiler)
		self.compiler = compilers[compiler](options)
		self.compiler.configure(platform)
		self._config.update(self.compiler.as_dict())
		self._config["CXX"] = self.compiler.get_bin()

		if platform not in platforms:
			raise Exception("Unsupported platform: " + platform)
		self.platform = platforms[platform]()
		self.platform.configure()
		self._config.update(self.platform.as_dict())

	def add_include_path(self, *paths):
		for inc in paths:
			self._config["CPPPATH"].append(inc)

	def resolve_include_path(self, inc):
		for inc_dir in self.platform.get_include_paths():
			fp = os.path.isdir(os.path.join(inc_dir, inc))
			if fp:
				self._config["CPPPATH"].append(inc)
				self._config["CPPPATH"] = list(set(self._config["CPPPATH"]))
		raise Exception("Missing headers for: %s" % inc)

	def add_library(self, *libs):
		for lib in libs:
			self._config["LIBS"].append(lib)
			for lib_dir in self.platform.get_lib_paths():
				fp = os.path.isdir(os.path.join(lib_dir, lib))
				if fp:
					self._config["LIBPATH"].append(lib_dir)
		self._config["LIBPATH"] = list(set(self._config["LIBPATH"]))

	def set_static_build(self):
		self._static_build = True


	def as_dict(self):
		if self._static_build:
			self._config["LINKFLAGS"].extend(
				self.compiler.get_static_linkflags()
			)
			self._config["LIBS"].extend(
				self.compiler.get_static_libs()
			)
			self._config["LINKFLAGS"].extend(
				self.platform.get_static_linkflags()
			)
			self._config["LIBS"].extend(
				self.platform.get_static_libs()
			)
		else:
			self._config["LINKFLAGS"].extend(
				self.compiler.get_linkflags()
			)
			self._config["LIBS"].extend(
				self.compiler.get_libs()
			)
			self._config["LINKFLAGS"].extend(
				self.platform.get_linkflags()
			)
			self._config["LIBS"].extend(
				self.platform.get_libs()
			)

		return self._config
