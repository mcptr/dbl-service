#ifndef DBL_SERVICE_UPDATER_HXX
#define DBL_SERVICE_UPDATER_HXX

#include "core/rtapi.hxx"

#include <memory>
#include <atomic>
#include <condition_variable>

namespace dbl {
namespace service {

class Updater
{
public:
	Updater() = delete;
	Updater(std::shared_ptr<RTApi> api);
	virtual ~Updater() = default;

	bool run();
	void stop();
	void enable_list_update(bool state = true);
protected:
	std::shared_ptr<RTApi> api_;

	std::atomic<bool> stop_flag_ {false};
	std::mutex mtx_;
	std::condition_variable cv_;

	bool enable_list_update_;

	bool update();
};

} // service
} // dbl

#endif
