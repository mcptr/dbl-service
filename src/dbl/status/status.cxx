#include "status.hxx"
#include "dbl/core/common.hxx"

namespace dbl {
namespace status {

Status::Status()
	: types::JSONSerializable()
{
	start_tstamp_ = time(nullptr);
	LOG(INFO) << "CONSTRUCTING NEW STATUS #################################################"
			  << start_tstamp_;
}

void Status::init_from_json(const Json::Value& /*root*/)
{
}

void Status::to_json(Json::Value& root) const
{
	root["start_time"] = int(start_tstamp_);
}


} // status
} // dbl

