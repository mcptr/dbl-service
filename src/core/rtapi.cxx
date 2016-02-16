#include "rtapi.hxx"

namespace dbl {

RTApi::RTApi(const Options& po)
	: program_options(po),
	  db(new DB(po.get<std::string>("db"), 5))
{
	//LOG(INFO) << 
}

} // dbl
