#ifndef DBL_STATUS_STATUS_HXX
#define DBL_STATUS_STATUS_HXX

#include "dbl/types/json_serializable.hxx"
#include <ctime>

namespace dbl {
namespace status {

class Status : public types::JSONSerializable
{
public:
	Status();
	virtual void init_from_json(const Json::Value& root);
	virtual void to_json(Json::Value& root) const;
private:
	std::time_t start_tstamp_;
};

} // status
} // dbl

#endif
