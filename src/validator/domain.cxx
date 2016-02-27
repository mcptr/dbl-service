#include "domain.hxx"
#include "core/common.hxx"

#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>

namespace dbl {
namespace validator {
namespace domain {

bool is_valid(const std::string& name)
{
	Errors_t errors;
	return is_valid(name, errors);
}

bool is_valid(const std::string& name, Errors_t& errors)
{
	errors.clear();
	struct addrinfo* result = nullptr;
	int error = getaddrinfo(name.c_str(), nullptr, nullptr, &result);

	if(error != 0) {
		if(error == EAI_SYSTEM) {
			errors.push_back(strerror(errno));
		}
		else {
			errors.push_back(gai_strerror(error));
		}
	}

	if(result != nullptr) {
		freeaddrinfo(result);
	}

	return (errors.size() == 0);
}


} // domain
} // validator
} // dbl
