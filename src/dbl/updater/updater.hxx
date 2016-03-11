#ifndef DBL_UPDATER_UPDATER_HXX
#define DBL_UPDATER_UPDATER_HXX

#include "dbl/core/api.hxx"
#include "dbl/net/net.hxx"

#include <memory>
#include <atomic>
#include <condition_variable>

namespace dbl {
namespace updater {

class Updater
{
public:
	Updater() = delete;
	Updater(std::shared_ptr<core::Api> api);
	virtual ~Updater() = default;

	bool run();
protected:
	std::shared_ptr<core::Api> api_;
	bool is_updated_;

	void update_lists();
};

} // updater
} // dbl

#endif
