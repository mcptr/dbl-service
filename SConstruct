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

AddOption(
	"--without-service",
	dest="disable_server_build",
	action="store_true",
	default=False,
	help="Do not build server")

AddOption(
	"--without-client",
	dest="disable_client_build",
	action="store_true",
	default=False,
	help="Do not client")

AddOption(
	"--with-tests",
	dest="build_tests",
	action="store_true",
	default=False,
	help="Build tests")


class Dirs(object):
	build = "#build"
	objects = "%s/objs" % build
	target = "%s/target" % build
	source = "#src"
	source_service = os.path.join("#src", "dbl")
	extern_include = os.path.join(os.environ["VIRTUAL_ENV"], "include")
	project_source = source
	destdir = "#bin"
	lib_destdir = "#lib"


MAIN_TARGET_NAME = "dnsblocker"
THIS_PLATFORM = os.uname()[0].lower()

builder_options = dict()
builder = Builder(builder_options, "clang")

if GetOption("debug_build"):
	builder.add_define("DEBUG")
	builder.set_debug_build()

builder.add_lib_path(
	os.path.join(os.environ["VIRTUAL_ENV"], "lib"),
	os.path.join(os.environ["VIRTUAL_ENV"], "lib64"),
)

builder.add_library(
	"boost_system",
	"boost_program_options",
	"boost_filesystem",
	"boost_date_time",
	"soci_core",
	"soci_sqlite3",
	"jsoncpp",
	"curl",
	"crypto",
)

builder.add_include_path(Dirs.source, Dirs.extern_include)

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

common_translation_units = {
	"dbl/types/json_serializable": {},
	"dbl/types/domain": {},
	"dbl/types/domain_list": {},
	"dbl/util/crypto" : {},
	"dbl/validator/domain": {},
	"dbl/net/http/request": {},
	"dbl/net/http/response": {},
	"dbl/util/http" : {},
}

server_translation_units = {
	"dbl/auth/auth": {},
	"dbl/core/api" : {},
	"dbl/db/db": {},
	"dbl/db/utils/utils": {},
	"dbl/dnsproxy/dnsmasq" : {},
	"dbl/dnsproxy/dnsproxy" : {},
	"dbl/dnsproxy/factory" : {},
	"dbl/dnsproxy/unbound" : {},
	"dbl/init/init" : {},
	"dbl/logger/logger" : {},
	"dbl/main" : {},
	"dbl/manager/domain_manager": {},
	"dbl/manager/domain_list_manager": {},
	"dbl/manager/manager": {},
	"dbl/manager/settings_manager": {},
	"dbl/mgmt/mgmt": {},
	"dbl/options/options" : {},
	"dbl/query/query" : {},
	"dbl/service/configurator/configurator" : {},
	"dbl/service/configurator/factory" : {},
	"dbl/service/server/connection" : {},
	"dbl/service/server/http_responder_connection" : {},
	"dbl/service/server/service_connection" : {},
	"dbl/service/service" : {},
	"dbl/service/worker/worker" : {},
	"dbl/status/status": {},
	"dbl/sys/command": {},
	"dbl/sys/script/script": {},
	"dbl/template/template": {},
	"dbl/updater/updater" : {},
	"dbl/util/fs" : {},
}

server_platform_translation_units = {
	"linux": {
		"dbl/config/unix": {},
		"dbl/service/configurator/unix" : {},
		"dbl/service/unix" : {},
		"dbl/service/worker/unix" : {},
	}
}

server_translation_units.update(server_platform_translation_units[THIS_PLATFORM])

common_target_objects = []
main_target_objects = []

for tunit in sorted(common_translation_units):
	tunit_def = common_translation_units[tunit]
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
	common_target_objects.append(obj)

if not GetOption("disable_server_build"):
	for tunit in sorted(server_translation_units):
		tunit_def = server_translation_units[tunit]
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
		source=[main_target_objects, common_target_objects]
	)

if not GetOption("disable_client_build"):
	SConscript(
		os.path.join(Dirs.source, "dblclient", "SConscript"),
		exports=[
			"extend_env", "env", "Dirs", "common_translation_units",
			"common_target_objects", "THIS_PLATFORM", 
		],
		variant_dir=os.path.join(Dirs.build, "dblclient"),
		duplicate=0
	)

if GetOption("build_tests"):
	test_env = extend_env(env, {
		"LIBS": ["cxxtestutil"]
	})
	test_sconscripts = {
		"dblclient": os.path.join(
			Dirs.source, "test", "dblclient", "SConscript"
		),
	}

	exports = [
		"extend_env", "env", "Dirs", "common_translation_units",
		"common_target_objects", "THIS_PLATFORM", 
	]

	for test_builder in test_sconscripts:
		SConscript(
			test_sconscripts[test_builder],
			exports=exports,
			variant_dir=os.path.join(Dirs.build, "tests", test_builder),
			duplicate=0
		)
