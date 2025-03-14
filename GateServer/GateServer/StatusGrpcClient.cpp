#include "StatusGrpcClient.h"
StatusGrpcClient::StatusGrpcClient()
{
	ConfigMgr& config_mgr = ConfigMgr::Inst();
	auto host = config_mgr["StatusServer"]["host"];
	auto port = config_mgr["StatusServer"]["port"];
	pool_.reset(new StatusConPool(5, host, port));
}

StatusGrpcClient::~StatusGrpcClient()
{

}

message::GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	grpc::ClientContext context;
	message::GetChatServerReq request;
	message::GetChatServerRsp response;
	request.set_uid(uid);

	auto con = pool_->getConnection();
	grpc::Status status = con->GetChatServer(&context, request, &response);

	if(status.ok())
	{
		pool_->returnConnection(std::move(con));
		return response;
	}
	else
	{
		pool_->returnConnection(std::move(con));
		response.set_error(ErrorCodes::RPCGetFailed);
		return response;
	}
}
message::LoginRsp StatusGrpcClient::Login(int uid, std::string token)
{
	grpc::ClientContext client_context;
	message::LoginReq request;
	message::LoginRsp reply;
	request.set_uid(uid);
	request.set_token(token);

	auto stub = pool_->getConnection();
	grpc::Status status = stub->Login(&client_context, request, &reply);
	if (!status.ok())
	{
		reply.set_error(ErrorCodes::RPCGetFailed);
	}
	pool_->returnConnection(std::move(stub));
	return reply;
}
