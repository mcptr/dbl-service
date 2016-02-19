import sys
import os
from collections import OrderedDict
from buildsys.builder import Builder

AddOption(
	"--compiler",
	dest="compiler",
	action="store",
	choices=["clang", "gcc"],
	default="clang",
	help="compiler to use")


AddOption(
	"--enable-debug",
	dest="debug_build",
	action="store_true",
	default=False,
	help="debug build")

AddOption(
	"--static-build",
	dest="static_build",
	action="store_true",
	default=False,
	help="link statically")


class Dirs(object):
	build = "#build"
	objects = "%s/objs" % build
	target = "%s/target" % build
	source = "#src"
	extern_source = os.path.join(Dir("#extern").path, "root", "include")
	project_source = source
	destdir = "bin"


MAIN_TARGET_NAME = "dnsblocker"
THIS_PLATFORM = os.uname()[0].lower()

builder = Builder({}, "clang")
builder.add_library(
	"boost_system",
	"boost_program_options",
	"boost_filesystem",
	"soci_core",
	"soci_sqlite3",
)

builder.add_include_path(Dirs.source, Dirs.extern_source)

build_options = {}

if GetOption("static_build"):
	builder.set_static_build()
	MAIN_TARGET_NAME += "-static"

env = Environment(**builder.as_dict())
env["ENV"]["TERM"] = os.environ.get("TERM")


def extend_env(dest, src):
	entities = ["CXXFLAGS", "CPPPATH", "LIBS", "LIBPATH"]
	src = src if isinstance(src, list) else [src]
	for skel in src:
		for item in entities:
				if not dest.get(item):
					dest[item] = []
				items = skel.get(item)
				if items:
					dest[item].extend(items if isinstance(items, list) else [items])
	for item in entities:
		# using OrderedDict, as set reorders elements causing linking error...
		dest[item] = OrderedDict.fromkeys(dest[item]).keys()
	return dest


translation_units = {
	"core/rtapi" : {},
	"db/db": {},
	"db/utils": {},
	"dnsproxy/base" : {},
	"dnsproxy/factory" : {},
	"dnsproxy/unbound/unbound" : {},
	"main" : {},
	"options/options" : {},
	"service/base" : {},
	"status/status" : {},
	"sys/command": {},
	"sys/script/base": {},
	"template/template": {},
	"util/fs" : {},
}

platform_translation_units = {
	"linux": {
		"config/unix": {},
		"dnsproxy/unbound/unix" : {},
		"service/unix" : {},
		"service/linux" : {},
		"sys/script/unix": {},
	}
}

translation_units.update(platform_translation_units[THIS_PLATFORM])


main_target_objects = []

for tunit in sorted(translation_units):
	tunit_def = translation_units[tunit]
	tunit_env = tunit_def.get("env", env).Clone()
	extend_env(tunit_env, {
		"CPPPATH" : tunit_def.get("cpppath", []),
		"LIBS" : tunit_def.get("libs", []),
		"LIBPATH" : tunit_def.get("libpath", []),
	})
	obj = tunit_env.SharedObject(
		os.path.join(Dirs.objects, tunit) + ".o",
		os.path.join(Dirs.project_source, tunit) + ".cxx"
	)
	main_target_objects.append(obj)

env.Program(
	target=os.path.join(Dirs.destdir, MAIN_TARGET_NAME),
	source=main_target_objects,
)
