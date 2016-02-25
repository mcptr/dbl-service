#include "command.hxx"
#include "core/common.hxx"

#include <cstring>
#include <errno.h>

namespace dbl {
namespace sys {

int run_command(const std::string& cmd, std::string& result)
{
	FILE* ptr(popen(cmd.c_str(), "r"));

	if(ptr == nullptr) {
		LOG(ERROR) << "popen() failed. " << strerror(errno)
				   << "\n" << cmd;
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
