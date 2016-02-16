#include "unix.hxx"
#include "core/common.hxx"

#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
 #include <sys/wait.h>

namespace dbl {

Script::Script(std::shared_ptr<RTApi> api, const std::string& name)
	: BaseScript(api, name)
{
}

bool Script::run()
{
	pid_t pid = fork();
	int status;

	if(pid < 0) {
		LOG(ERROR) << "Script::run(): fork() failed";
		return false;
	}
	else if(pid == 0) {
		constexpr int MAX_ENV = 32;
		char* environ[MAX_ENV];
		char* argv[] = {NULL};
		int i = 0;
		for(auto item : env_) {
			std::string var(item.first + "=" + item.second);
			strncpy(environ[i], var.c_str(), var.length());
			i++;
			if(i == MAX_ENV) {
				break;
			}
		}
		environ[i] = NULL;
		LOG(INFO) << "excvpe(): " << path_;
		if(execvpe(path_.c_str(), argv, environ)) {
			perror("execvpe()");
			LOG(ERROR) << "Script failed: " << path_;
			return false;
		}
	}
	else {
		wait(&status);
		int success = WEXITSTATUS(status);
		if(success != 0) {
			return false;
		}
		
		return true;
	}
	return false;
}

} // dbl
