#include "base.hxx"
#include "core/common.hxx"

#include <cstdlib>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace dbl {

BaseScript::BaseScript(std::shared_ptr<RTApi> api, const std::string& name)
	: api_(api),
	  name_(name)
{
	using boost::filesystem::path;
	using boost::filesystem::exists;
	std::string platform = api_->program_options.get<std::string>("platform");
	boost::algorithm::to_lower(platform);
	path basedir = api_->program_options.get<std::string>("basedir");
	path scriptdir = api_->program_options.get<std::string>("scriptdir");
	path platform_path = platform;
	path script_path =  basedir / scriptdir / platform_path / name_;

	if(!exists(script_path)) {
		std::string msg("Script not found: " + script_path.string());
		throw std::runtime_error(msg);
	}
	else {
		path_ = script_path.string();
	}

}

void BaseScript::set_env(const std::string& var, const std::string& value)
{
	env_[var] = value;
}

} // dbl
