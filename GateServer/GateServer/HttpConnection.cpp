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
		//�ж��Ƿ�������ֺ���ĸ���
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
			//��ȥ�����ַ�����ַ���Ҫ��ǰ��%���Ҹ���λ�͵���λ�ֱ�תΪ16���ơ�
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
		//��ԭ+Ϊ��
		if (str[i] == '+')
		{
			strTemp << " ";
		}
		//����%Ҫ������������ַ���16����ת��char��ƴ�ӡ�
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

				//��ȡ������֮����д���
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
	//���÷�����Ϣ�ĳ��ȣ����ڵײ��а�������
	_response.content_length(_response.body().size());
	http::async_write(
		_socket,
		_response,
		[self](beast::error_code ec, std::size_t)
		{
			//����˷�����Ϣʧ�ܣ���رշ��Ͷˡ�
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			//ȡ����ʱ������ֹ��ʱ��������ʱ��
			self->_deadline.cancel();
		});
}

void HttpConnection::HandleReq()
{
	//���ð汾
	_response.version(_request.version());
	//����Ϊ������
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
		//�洢���ĸ������������ķ�����
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
		//�洢���ĸ������������ķ�����
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}

void HttpConnection::PreParseGetParam()
{
	//��ȡurl
	auto url = _request.target();
	//���Ҳ�ѯ�ַ����Ŀ�ʼλ��(��'?'��λ��)
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
	//�������һ��������(���û��&�ָ���)
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