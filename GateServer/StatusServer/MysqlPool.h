#pragma once
#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <mysql/jdbc.h>
class MysqlPool
{
public:
	MysqlPool(const std::string& url, const std::string& user, const std::string& pass,
		const std::string& schema, int poolSize);

	std::unique_ptr<sql::Connection> getConnection();

	void returnConnection(std::unique_ptr<sql::Connection> con);

	void Close();

	~MysqlPool();
private:
	std::string url_;
	std::string user_;
	std::string pass_;
	std::string schema_;
	int pool_size_;
	std::atomic<bool> b_stop_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::queue<std::unique_ptr<sql::Connection>> pool_;
};

