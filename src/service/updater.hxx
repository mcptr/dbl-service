#ifndef DBL_SERVICE_UPDATER_HXX
#define DBL_SERVICE_UPDATER_HXX

#include "core/rtapi.hxx"


namespace dbl {

class Updater
{
public:
	Updater() = delete;
	Updater(std::shared_ptr<RTApi> api);
	~Updater() = default;

	void run();
private:
	std::shared_ptr<RTApi> api_;
	
};

} // dbl

#endif
