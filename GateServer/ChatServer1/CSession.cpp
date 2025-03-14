#include "CSession.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "CServer.h"
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context& ioc, CServer* server): socket_(ioc),
                                                                   server_(server),b_close_(false), uuid_(0), que_max_(50)
{
	recv_head_node_ = std::make_shared<Message>(head_total_len);
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
	session_id_ = boost::uuids::to_string(a_uuid);
}
boost::asio::ip::tcp::socket& CSession::getSocket()
{
	return socket_;
}
std::string CSession::getSessionId()
{
	return session_id_;
}

void CSession::asyncWrite(char* data, short length, short msg_id)
{
	std::unique_lock lock(write_mutex_);
	if(write_que_.size() > que_max_)
	{
		std::cout << "wtite que is full, size is: " << que_max_ << "\n";
		return;
	}

	write_que_.push(std::make_shared<Message>(data, length, msg_id));
	if(write_que_.size() <= 0)
	{
		std::cout << "write que push error\n";
		return;
	}
	auto msg = write_que_.front();
	write_que_.pop();
	lock.unlock();
	boost::asio::async_write(socket_, boost::asio::buffer(msg->data_, msg->total_len_),
		std::bind(&CSession::writeHandler, this, 
			std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void CSession::asyncWrite(const std::string& data, int msg_id)
{
	std::unique_lock lock(write_mutex_);
	if (write_que_.size() > que_max_)
	{
		std::cout << "wtite que is full, size is: " << que_max_ << "\n";
		return;
	}

	write_que_.push(std::make_shared<Message>(data.c_str(), data.length(), msg_id));
	if (write_que_.size() == 0)
	{
		std::cout << "write que push error\n";
		return;
	}
	auto msg = write_que_.front();
	write_que_.pop();
	lock.unlock();
	boost::asio::async_write(socket_, boost::asio::buffer(msg->data_, msg->total_len_),
		std::bind(&CSession::writeHandler, this, 
			std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void CSession::asyncReadHead()
{
	auto self = shared_from_this();
	asyncReadFull(head_total_len, [self, this](const boost::system::error_code& ec, size_t bytes_transfered)
		{
			try
			{
				if (ec)
				{
					std::cout << "handle read failed, error is: " << ec.what() << "\n";
					close();
					server_->clearSession(session_id_);
					return;
				}
				
				recv_head_node_->Clear();
				::memcpy(recv_head_node_->data_, data_, head_total_len);

				//处理头部信息
				unsigned short msg_id = 0;
				memcpy(&msg_id, recv_head_node_->data_, head_id_len);
				msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
				std::cout << "msg_id is:" << msg_id << "\n";

				//处理消息长度
				unsigned short msg_len = 0;
				::memcpy(&msg_len, recv_head_node_->data_ + head_id_len, head_total_len - head_id_len);
				msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
				std::cout << "msg len is: " << msg_len << "\n";
				if (msg_len > max_length)
				{
					std::cout << "message length out of the range.\n";
					std::cout << "len should less then " << max_length << "but in fact:" << msg_len << "\n";
					close();
					server_->clearSession(session_id_);
					return;
				}
				recv_msg_node_ = std::make_shared<Message>(msg_len);
				
				asyncReadBody(msg_len);
			}
			catch (std::exception& e)
			{
				std::cout << "asyncReadFull error: " << e.what() << "\n";
			}
		});
}

void CSession::asyncReadBody(unsigned short msg_len)
{
	auto self = shared_from_this();
	asyncReadFull(msg_len, [self, this, msg_len](const boost::system::error_code& ec, size_t bytes_transfered)
		{
			try
			{
				if (ec)
				{
					std::cout << "asyncReadBody error: " << ec.what() << "\n";
					close();
					server_->clearSession(session_id_);
					return;
				}

				//将消息读取入消息体中
				::memcpy(recv_msg_node_->data_, data_, msg_len);
				recv_msg_node_->cur_len_ += msg_len;
				recv_msg_node_->data_[recv_msg_node_->total_len_] = '\0';
				std::cout << "receive message is:" << recv_msg_node_->data_ << "\n";
				LogicSystem::GetInstance()->postMsgToQue(std::make_shared<LogicNode>(self, recv_msg_node_));

				asyncReadHead();
			}
			catch (std::exception& e)
			{
				std::cout << "error code is:" << e.what() << "\n";
			}
			
		});
}

void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	::memset(data_, 0, max_length);
	asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	auto self = shared_from_this();
	socket_.async_read_some(
		boost::asio::buffer(data_ + read_len, total_len - read_len),
		[self, read_len, total_len, handler]
		(const boost::system::error_code& ec, size_t bytes_transferred)
		{
			if (ec)
			{
				//出现错误，则使用回调函数处理
				std::cout << "async read len error: " << ec.what() << "\n";
				handler(ec, bytes_transferred);
				return;
			}
			if (read_len + bytes_transferred >= total_len)
			{
				//读取到足够长度，做回调处理。
				handler(ec, read_len + bytes_transferred);
				return;
			}
			//没有错误且长度不足，则继续读取
			self->asyncReadLen(read_len + bytes_transferred, total_len, handler);
		});
}

void CSession::writeHandler(const boost::system::error_code& error_code, size_t bytes_transferred, std::shared_ptr<CSession> self)
{
	try
	{
		if(!error_code)
		{
			std::unique_lock lock(write_mutex_);
			if(!write_que_.empty())
			{
				auto msg = write_que_.front();
				write_que_.pop();
				lock.unlock();
				boost::asio::async_write(socket_, boost::asio::buffer(msg->data_, msg->total_len_),
					std::bind(&CSession::writeHandler, this,
						std::placeholders::_1, std::placeholders::_2, shared_from_this()));
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "write handler error, error is: " << e.what() << "\n";
	}
}
