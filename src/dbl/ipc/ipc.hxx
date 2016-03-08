#ifndef DBL_IPC_IPC_HXX
#define DBL_IPC_IPC_HXX

#include "dbl/core/common.hxx"

#include <string>
#include <set>
#include <memory>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/offset_ptr.hpp>

namespace dbl {
namespace ipc {

namespace bipc = boost::interprocess;

template <class T>
class SharedMemory
{
public:
	SharedMemory() = delete;
	SharedMemory(const std::string& name, std::size_t size = 65535);
	virtual ~SharedMemory();
	T* get_object();
private:
	const std::string name_;
	std::unique_ptr<bipc::managed_shared_memory> segment_;
	bipc::offset_ptr<T> object_ptr_;
};

template <class T>
SharedMemory<T>::SharedMemory(const std::string& name, std::size_t size)
	: name_(name)
{
	segment_.reset(
		new bipc::managed_shared_memory(
			bipc::open_or_create, 
			name_.c_str(),
			size
		)
	);
	object_ptr_ = static_cast<T*>(segment_->allocate(sizeof(T)));
}

template <class T>
SharedMemory<T>::~SharedMemory()
{
	LOG(DEBUG) << "Deallocating shared memory";
	segment_->deallocate(object_ptr_.get());
	LOG(DEBUG) << "Removing shared memory object";
	bipc::shared_memory_object::remove(name_.c_str());
}

template <class T>
T* SharedMemory<T>::get_object()
{
	return object_ptr_.get();
}

} // ipc
} // dbl


#endif
