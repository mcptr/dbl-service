#ifndef DBL_SERVICE_WORKER_UNIX_HXX
#define DBL_SERVICE_WORKER_UNIX_HXX
#include "worker.hxx"

namespace dbl {
namespace service {
namespace worker {

class Unix : public Worker
{
public:
	Unix() = delete;
	Unix(std::shared_ptr<dbl::core::Api> api);
	virtual ~Unix() = default;

	virtual void run(std::condition_variable& cv);
protected:
	virtual void drop_privileges();
};

} // worker
} // service
} // dbl

#endif
