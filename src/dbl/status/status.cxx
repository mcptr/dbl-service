#include "status.hxx"
#include "dbl/core/common.hxx"
#include "dbl/core/constants.hxx"

#include <sys/utsname.h>

namespace dbl {
namespace status {

Status::Status()
	: types::JSONSerializable()
{
}

void Status::init()
{
	start_tstamp = time(nullptr);
	version = dbl::core::constants::VERSION;

	struct utsname uts;
	if(uname(&uts) == 0) {
		os = uts.sysname;
		os.append(" ");
		os.append(uts.release);
		os.append(" ");
		os.append(uts.machine);

		nodename.append(uts.nodename);
		nodename.append(" ");
	}
}

void Status::init_from_json(const Json::Value& root)
{
	if(!root.isObject()) {
		throw std::runtime_error("Invalid format");
	}

	start_tstamp = root["start_time"].asInt();
	version = root["version"].asString();
	os = root["os"].asString();
	nodename = root["nodename"].asString();
	total_blocked_domains = root["blocked_domains"].asInt();
	total_lists = root["domain_lists"].asInt();
}

void Status::to_json(Json::Value& root) const
{
	root["start_time"] = int(start_tstamp);
	root["version"] = version;
	root["os"] = os;
	root["nodename"] = nodename;
	root["domain_lists"] = total_lists;
	root["blocked_domains"] = total_blocked_domains;
}

} // status
} // dbl

