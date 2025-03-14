#include "MysqlPool.h"

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& pass,
	const std::string& schema, int poolSize)
	:url_(url), user_(user), pass_(pass),
	schema_(schema), pool_size_(poolSize), b_stop_(false)
{
	try
	{
		for (int i = 0; i < pool_size_; i++)
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, pass_));
			con->setSchema(schema_);
			pool_.push(std::move(con));

		}
	}
	catch (sql::SQLException& error)
	{
		/*
		 * 处理异常的逻辑
		 */

		std::cout << "mysql pool init failed, error is:" << error.what() << "\n";
	}
}

std::unique_ptr<sql::Connection> MysqlPool::getConnection()
{
	std::unique_lock lock(mutex_);
	condition_.wait(lock, [this]()
		{
			if (b_stop_)
			{
				return true;
			}
			return !pool_.empty();
		});
	if (b_stop_)
	{
		return nullptr;
	}
	std::unique_ptr<sql::Connection> con(std::move(pool_.front()));
	pool_.pop();
	return con;
}

void MysqlPool::returnConnection(std::unique_ptr<sql::Connection> con)
{
	std::unique_lock lock(mutex_);
	if (b_stop_)
	{
		return;
	}
	pool_.push(std::move(con));
	condition_.notify_one();
}

void MysqlPool::Close()
{
	b_stop_ = true;
	condition_.notify_all();//唤醒所有正在等待的线程。
}

MysqlPool::~MysqlPool()
{
	std::unique_lock lock(mutex_);
	while (!pool_.empty())
	{
		pool_.pop();//解除所有连接
	}
}
