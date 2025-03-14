#include "HttpConnection.h"
#include "LogicSystem.h"

std::string ToHex(unsigned char number)
{
	std::stringstream ss;
	ss << std::hex << number;
	std::string hex;
	ss >> hex;
	return hex;
}

int FromHex(unsigned char hex)
{
	std::stringstream ss;
	ss << hex;
	int number{};
	ss >> std::hex >> number;
	return number;
}

std::string UrlEncode(const std::string& str)
{
	std::stringstream strTemp{};
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//判断是否仅有数字和字母组成
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
		{
			strTemp << str[i];
		}
		else if (str[i] == ' ')
			strTemp << "+";
		else
		{
			//除去上述字符外的字符需要提前加%并且高四位和低四位分别转为16进制。
			strTemp << '%';
			strTemp << ToHex((unsigned char)str[i] >> 4);
			strTemp << ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp.str();
}

std::string UrlDecode(const std::string& str)
{
	std::stringstream strTemp{};
	size_t length = str.length();
	for(size_t i = 0; i < length; i++)
	{
		//还原+为空
		if (str[i] == '+')
		{
			strTemp << " ";
		}
		//遇到%要将后面的两个字符从16进制转成char再拼接。
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp << high * 16 + low;
		}
		else strTemp << str[i];
	}
	return strTemp.str();
}

HttpConnection::HttpConnection(boost::asio::io_context& ioc):_socket(ioc)
{

}

void HttpConnection::Start()
{
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request,
		[self](beast::error_code ec, std::size_t bytes_transferred) {
			try
			{
				if (ec)
				{
					std::cout << "http read err is: " << ec.what() << "\n";
					return;
				}

				//读取到内容之后进行处理
				boost::ignore_unused(bytes_transferred);
				self->HandleReq();
				self->CheckDeadline();
			}
			catch (std::exception& exp)
			{
				std::cout << "exception is: " << exp.what() << "\n";
			}
		});
}

void HttpConnection::CheckDeadline()
{
	auto self = shared_from_this();
	_deadline.async_wait([self](beast::error_code ec)
		{
			if (!ec)
			{
				self->_socket.close();
			}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	//设置发送消息的长度，便于底层切包操作。
	_response.content_length(_response.body().size());
	http::async_write(
		_socket,
		_response,
		[self](beast::error_code ec, std::size_t)
		{
			//服务端发送消息失败，则关闭发送端。
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			//取消定时器，防止定时器持续计时。
			self->_deadline.cancel();
		});
}

void HttpConnection::HandleReq()
{
	//设置版本
	_response.version(_request.version());
	//设置为短连接
	_response.keep_alive(false);

	if(_request.method() == http::verb::get)
	{
		PreParseGetParam();
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());

		if(!success)
		{
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}

		_response.result(http::status::ok);
		//存储是哪个服务器做出的反馈。
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}

	if (_request.method() == http::verb::post)
	{
		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());

		if (!success)
		{
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}

		_response.result(http::status::ok);
		//存储是哪个服务器做出的反馈。
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}

void HttpConnection::PreParseGetParam()
{
	//提取url
	auto url = _request.target();
	//查找查询字符串的开始位置(即'?'的位置)
	auto query_pos = url.find("?");
	if (query_pos == std::string::npos)
	{
		_get_url = url;
		return;
	}

	_get_url = url.substr(0, query_pos);
	std::string query_string = url.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while((pos = query_string.find('&')) != std::string::npos)
	{
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos)
		{
			key = UrlDecode(pair.substr(0, eq_pos));
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	//处理最后一个参数对(如果没有&分隔符)
	if (!query_string.empty())
	{
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos)
		{
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}