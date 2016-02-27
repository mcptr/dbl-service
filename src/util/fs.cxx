#include "fs.hxx"
#include "core/common.hxx"

#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace dbl {
namespace util {
namespace fs {

std::string find_executable(const std::string& name)
{

#if defined(__linux) ||  defined(__freebsd) || defined(__unix)
	std::vector<std::string> dirs = {
		"/bin",
		"/sbin",
		"/usr/bin",
		"/usr/sbin",
		"/usr/local/bin",
		"/usr/local/sbin",
	};

	struct stat st;

	for(auto const& dir : dirs) {
		std::string file_path = dir + "/" + name;
		if(stat(file_path.c_str(), &st) == 0) {
			if(st.st_mode & S_IXUSR) {
				return file_path;
			}
		}
	}

#endif

	return "";
}

} // fs
} // util
} // dbl
