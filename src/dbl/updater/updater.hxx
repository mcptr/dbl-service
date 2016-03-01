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
	void stop();
	void enable_list_update(bool state = true);
protected:
	std::shared_ptr<core::Api> api_;

	std::atomic<bool> stop_flag_ {false};
	std::mutex mtx_;
	std::condition_variable cv_;

	bool enable_list_update_;
	bool is_updated_ = false;

	void update();
	void update_lists();
};

} // updater
} // dbl

#endif
