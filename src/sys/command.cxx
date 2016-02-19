#include "command.hxx"
#include "core/common.hxx"

namespace dbl {

int run_command(const std::string& cmd, std::string& result)
{
    FILE* ptr(popen(cmd.c_str(), "r"));

	if(ptr == nullptr) {
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

} // dbl

