#include "service_response.hxx"

namespace dblclient {
namespace net {

ServiceResponse::ServiceResponse(const std::string& raw_response)
{
	Json::Reader reader;
	is_ok_ = reader.parse(raw_response, data_);
}

bool ServiceResponse::is_ok() const
{
	return is_ok_;
}


const Json::Value& ServiceResponse::get_data() const
{
	return data_;
}

} // net
} // dblclient
