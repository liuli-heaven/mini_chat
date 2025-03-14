#include "IOServicePool.h"
#include <iostream>
IOServicePool::~IOServicePool()
{
	Stop();
	std::cout << "IOServicePool destruct\n";
}
boost::asio::io_context& IOServicePool::GetIOService()
{
	auto& service = _IOServices[_nextIOService++];
	if (_nextIOService == _IOServices.size())
	{
		_nextIOService = 0;
	}
	return service;
}

void IOServicePool::Stop()
{
	//仅仅执行work.reset无法让iocontext停止，需要手动stop来停止。
	for (auto& work : _works)
	{
		work->get_io_context().stop();
		work.reset();
	}
	for (auto& t : _threads)
	{
		t.join();
	}
}

IOServicePool::IOServicePool(std::size_t size): _IOServices(size), _works(size), _nextIOService(0)
{
	for (size_t i = 0; i < size; i++)
	{
		_works[i] = std::unique_ptr<Work>(new Work(_IOServices[i]));
	}

	for (size_t i = 0; i < _IOServices.size(); i++)
	{
		_threads.emplace_back([this, i]()
			{
				_IOServices[i].run();
			});
	}
}