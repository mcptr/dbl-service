#ifndef DBL_AUTH_AUTH_HXX
#define DBL_AUTH_AUTH_HXX

#include "core/api.hxx"
#include "types/types.hxx"

#include <string>
#include <set>
#include <memory>

namespace dbl {
namespace auth {

class Auth
{
public:
	Auth() = delete;
	Auth(std::shared_ptr<core::Api> api);
	~Auth() = default;

	bool auth(const std::string& token,
			  const std::string& hash);

	bool set_password(const std::string& passwd_hash,
					  const std::string& hashed_token,
					  types::Errors_t& errors);

	bool remove_password();

	inline const std::string& get_token() const
	{
		return token_;
	}
private:
	std::shared_ptr<core::Api> api_;
	std::string token_;
};

} // auth
} // dbl

#endif
