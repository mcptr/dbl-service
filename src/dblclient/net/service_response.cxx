#include "service_response.hxx"
#include <iostream>

namespace dblclient {
namespace net {

ServiceResponse::ServiceResponse(const std::string& raw_response)
{
	Json::Reader reader;
	is_ok_ = reader.parse(raw_response, data_);
	if(is_ok_) {
		is_ok_ = data_["success"].asBool();
		if(!is_ok_) {
			error_msg_ = data_["error_message"].asString();
		}
	}
}

bool ServiceResponse::is_ok() const
{
	return is_ok_;
}

const Json::Value& ServiceResponse::get_data() const
{
	return data_;
}

const std::string& ServiceResponse::get_error_msg() const
{
	return error_msg_;
}

} // net
} // dblclient
