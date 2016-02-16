#ifndef DBL_DNSPROXY_BASE_HXX
#define DBL_DNSPROXY_BASE_HXX

#include "core/rtapi.hxx"

#include <set>
#include <vector>
#include <string>
#include <unordered_map>

namespace dbl {

class DNSProxy
{
public:
	typedef std::set<std::string> DomainList_t;
	typedef std::vector<std::string> ConfigEntries_t;

	DNSProxy() = delete;
	DNSProxy(std::shared_ptr<RTApi> api);
	virtual ~DNSProxy() = default;

	virtual void add_domain(const std::string& name) final;
	virtual void remove_domain(const std::string& name) final;
	virtual std::size_t count_domains() final;

	virtual std::string find_executable() const final;
	virtual std::string get_executable_name() const = 0;
	virtual void create_config() = 0;

	// virtual bool start();
	// virtual bool stop();
	// virtual bool is_running();

protected:
	std::shared_ptr<RTApi> api_;
	ConfigEntries_t config_;
	DomainList_t domains_;

	virtual void generate_config() = 0;
private:
};

} // dbl

#endif
