#pragma once
#include <memory>
#include <mutex>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <iostream>
template<class T>
class Singleton
{
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	~Singleton()
	{
		std::cout << "this is singleton destruct" << std::endl;
	}

	static std::shared_ptr<T> GetInstance()
	{
		static std::once_flag once_flag;
		std::call_once(once_flag, [&]() {
			_instance = std::shared_ptr<T>(new T);
			});
		return _instance;
	}

	void PrintAddress()
	{
		std::cout << _instance.get() << std::endl;
	}
protected:
	Singleton() = default;

	static std::shared_ptr<T> _instance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

