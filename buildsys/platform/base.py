class Base(object):
	def configure(self):
		pass

	def get_include_paths(self):
		return []

	def get_lib_paths(self):
		return []

	def get_static_lib_paths(self):
		return []

	def get_libs(self):
		return []

	def get_static_libs(self):
		return []

	def get_linkflags(self):
		return []

	def get_static_linkflags(self):
		return []

	def as_dict(self, **kwargs):
		libpath = ["extern/root/lib", "extern/root/lib64"]
		libpath.extend(self.get_lib_paths())
		return dict(
			LIBPATH=libpath,
			CPPPATH=self.get_include_paths(),
			LINKFLAGS=self.get_linkflags()
		)
