#include "command.hxx"
#include "dbl/core/common.hxx"

#include <cstdio>

namespace dbl {
namespace sys {

int run_command(const std::string& cmd, std::string& result)
{
	FILE* ptr(popen(cmd.c_str(), "r"));

	if(ptr == nullptr) {
		PLOG(ERROR) << "popen()";
		LOG(ERROR) << "Command failed:" << cmd;
		return -1;
	}

	char buf[128];
	result.clear();
	while(!feof(ptr)) {
		if(fgets(buf, 128, ptr) != NULL)
			result.append(buf);
	}

	return pclose(ptr);
}

} // sys
} // dbl
