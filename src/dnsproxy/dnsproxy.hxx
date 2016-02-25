#ifndef DBL_DNSPROXY_DNSPROXY_HXX
#define DBL_DNSPROXY_DNSPROXY_HXX

#include "core/api.hxx"
#include "template/template.hxx"

#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <unistd.h>

namespace dbl {
namespace dnsproxy {

class DNSProxy
{
public:
	typedef std::set<std::string> DomainList_t;
	typedef Template::Values_t ConfigEntries_t;

	typedef enum { IPv4, IPv6 } Protocol_t;


	DNSProxy() = delete;
	DNSProxy(std::shared_ptr<core::Api> api);
	virtual ~DNSProxy() = default;

	virtual void add_domain(const std::string& name) final;
	virtual void remove_domain(const std::string& name) final;
	virtual std::size_t count_domains() final;

	virtual void create_config() = 0;

	virtual void set_value(const std::string& k,
						   const std::string& v) final;

	virtual inline const std::string& get_config_path() const final
	{
		return config_file_path_;
	}

protected:
	std::shared_ptr<core::Api> api_;
	ConfigEntries_t config_;
	DomainList_t domains_;

	std::string config_file_path_;

	virtual void generate_config();

};

} // dnsproxy
} // dbl

#endif
