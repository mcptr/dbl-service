#include "base.hxx"
#include "core/common.hxx"

#include <cstdlib>
#include <boost/filesystem.hpp>

namespace dbl {

BaseScript::BaseScript(std::shared_ptr<RTApi> api, const std::string& name)
	: api_(api),
	  name_(name)
{
}

void BaseScript::set_env(const std::string& var, const std::string& value)
{
	env_[var] = value;
}

bool BaseScript::is_success() const
{
	return (status_ == 0);
}

int BaseScript::get_status() const
{
	return status_;
}

bool BaseScript::run()
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

int BaseScript::execute() const
{
	return system(path_.c_str());
}

} // dbl
