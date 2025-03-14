#include "VerifyGrpcClient.h"

RPConPool::RPConPool(size_t poolSize, std::string host, std::string port)
	:poolSize_(poolSize), host_(host), port_(port), b_stop_(false)
{
	for (size_t i = 0; i < poolSize_; i++)
	{
		std::shared_ptr <grpc::Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());

		connections_.push(message::VarifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty())
	{
		connections_.pop();
	}
}
void RPConPool::Close()
{
	b_stop_ = true;
	condition_.notify_all();
}

std::unique_ptr<message::VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]
		{
			if (b_stop_)
			{
				return true;
			}
			return !connections_.empty();
		});
	if (b_stop_)
	{
		return nullptr;
	}
	auto context = std::move(connections_.front());
	connections_.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<message::VarifyService::Stub> context)
{
	std::lock_guard lock(mutex_);
	if (b_stop_)
	{
		return;
	}
	connections_.push(std::move(context));
	condition_.notify_one();
}

message::GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email)
{
	grpc::ClientContext context;
	message::GetVarifyRsp reply;
	message::GetVarifyReq request;
	request.set_email(email);
	auto stub = pool_->getConnection();
	grpc::Status status = stub->GetVarifyCode(&context, request, &reply);
	if (status.ok())
	{
		pool_->returnConnection(std::move(stub));
		return reply;
	}
	else
	{
		pool_->returnConnection(std::move(stub));
		reply.set_error(ErrorCodes::NetworkError);
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient()
{
	auto gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["host"];
	std::string port = gCfgMgr["VarifyServer"]["port"];
	pool_.reset(new RPConPool(5, host, port));
}