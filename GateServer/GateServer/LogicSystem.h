#pragma once
#include <functional>
#include <map>
#include "const.h"
#include "Singleton.h"

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;
class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem() = default;
	bool HandleGet(std::string path, std::shared_ptr<HttpConnection> con);
	void RegGet(std::string, HttpHandler handler);
	bool HandlePost(std::string path, std::shared_ptr<HttpConnection> con);
	void RegPost(std::string url, HttpHandler handler);
private:
	LogicSystem();
	//处理post请求的回调函数
	std::map<std::string, HttpHandler> _post_handlers;
	//处理get请求的回调函数
	std::map<std::string, HttpHandler> _get_handlers;
};

