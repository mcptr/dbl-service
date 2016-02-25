#include "script.hxx"
#include "core/common.hxx"

#include <cstdlib>
#include <boost/filesystem.hpp>

namespace dbl {
namespace sys {
namespace script {

Script::Script(std::shared_ptr<core::Api> api, const std::string& name)
	: api_(api),
	  name_(name)
{
}

void Script::set_env(const std::string& var, const std::string& value)
{
	env_[var] = value;
}

bool Script::is_success() const
{
	return (status_ == 0);
}

int Script::get_status() const
{
	return status_;
}

bool Script::run()
{
	using boost::filesystem::path;
	using boost::filesystem::exists;
	std::string platform = api_->config.platform;

	path base_dir = api_->config.base_dir;
	path script_dir = api_->config.script_dir;
	path script_path =  base_dir / script_dir;
	script_path /= platform;
	script_path /= name_;

	if(!exists(script_path)) {
		std::string msg("Script not found: " + script_path.string());
		throw std::runtime_error(msg);
	}

	path_ = script_path.string();

	this->export_env();
	status_ = this->execute();
	this->unexport_env();

	return this->is_success();
}

int Script::execute() const
{
	return system(path_.c_str());
}

} // script
} // sys
} // dbl
