#include "MysqlDAO.h"

#include "ConfigMgr.h"

MysqlDAO::MysqlDAO()
{
	auto& cfg = ConfigMgr::Inst();
	auto&& host = cfg["MySQL"]["host"];
	auto&& port = cfg["MySQL"]["port"];
	auto&& password = cfg["MySQL"]["password"];
	auto&& schema = cfg["MySQL"]["schema"];
	auto&& user = cfg["MySQL"]["user"];
	pool_.reset(new MysqlPool(host + ":" + port, user, password, schema, 5));
}

MysqlDAO::~MysqlDAO()
{
	pool_->Close();
}

int MysqlDAO::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	auto con = pool_->getConnection();
	try
	{
		if(con == nullptr)
		{
			pool_->returnConnection(std::move(con));
			return false;
		}
		//׼�����ô洢����
		std::unique_ptr<sql::PreparedStatement> statement(
			con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		//�����������
		statement->setString(1, name);
		statement->setString(2, email);
		statement->setString(3, pwd);

		//����preparedStatement��֧��ֱ��ע�������������Ҫʹ�ûỰ������������������ȡ���������ֵ

		//ִ�д洢����
		statement->execute();
		//����洢���������˻Ự������������������ʽ��ȡ���������ֵ������������ִ��SELECT����ѯ
		//���������@result��
		std::unique_ptr<sql::Statement> statementResult(con->createStatement());
		std::unique_ptr<sql::ResultSet> result(
			statementResult->executeQuery("SELECT @result AS result"));
		if (result->next())
		{
			int res = result->getInt("result");
			std::cout << "Result: " << res << "\n";
			pool_->returnConnection(std::move(con));
			return res;
		}
		pool_->returnConnection(std::move(con));
		return -1;
	}
	catch (sql::SQLException& error)
	{
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << error.what() << "\n";
		std::cerr << "(MySQL error code: " << error.getErrorCode() <<
			", SQLState: " << error.getSQLState() << ")\n";
		return -1;
	}
}

bool MysqlDAO::CheckEmail(const std::string& name, const std::string& email)
{
	auto con = pool_->getConnection();
	try
	{
		if (con == nullptr)
		{
			pool_->returnConnection(std::move(con));
			return false;
		}
		//׼����ѯ���
		std::unique_ptr<sql::PreparedStatement> statement(
			con->prepareStatement("SELECT email FROM users WHERE name = ?"));
		//���ò���
		statement->setString(1, name);
		//ִ��sql��䲢�洢���
		std::unique_ptr<sql::ResultSet> result(statement->executeQuery());

		//�������
		while (result->next())
		{
			std::cout << "check Email: " << result->getString("email") << "\n";
			if (email != result->getString("email"))
			{
				pool_->returnConnection(std::move(con));
				return false;
			}
		}
		pool_->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& error)
	{
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << error.what() << "\n";
		std::cerr << "(MySQL error code: " << error.getErrorCode() <<
			", SQLState: " << error.getSQLState() << ")\n";
		return false;
	}

}

bool MysqlDAO::UpdatePassword(const std::string& name, const std::string& password)
{
	auto con = pool_->getConnection();
	try
	{
		
		if (con == nullptr)
		{
			pool_->returnConnection(std::move(con));
			return false;
		}
		//׼���洢�޸Ĺ���
		std::unique_ptr<sql::PreparedStatement> statement(
			con->prepareStatement("UPDATE users SET password = ? WHERE name = ?")
		);
		//���ò���
		statement->setString(1, password);
		statement->setString(2, name);
		//ִ��
		int update_count = statement->executeUpdate();

		std::cout << "update rows:" << update_count << "\n";
		pool_->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& error)
	{
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << error.what() << "\n";
		std::cerr << "(MySQL error code: " << error.getErrorCode() <<
			", SQLState: " << error.getSQLState() << ")\n";
		return false;
	}
}

bool MysqlDAO::CheckPassword(const std::string& name, const std::string& password, UserInfo& userInfo)
{
	auto con = pool_->getConnection();
	try
	{
		if(con == nullptr)
		{
			pool_->returnConnection(std::move(con));
			return false;
		}
		std::unique_ptr<sql::PreparedStatement> statement(
			con->prepareStatement("SELECT * FROM users WHERE name = ?"));
		statement->setString(1, name);

		std::unique_ptr<sql::ResultSet> result(statement->executeQuery());
		std::string password_query;
		while (result->next())
		{
			password_query = result->getString("password");
			std::cout << "query password is:" << password_query << "\n";
			break;
		}
		if(password_query != password)
		{
			return false;
		}
		userInfo.name = name;
		userInfo.email = result->getString("email");
		userInfo.password = password;
		userInfo.uid = result->getInt("uid");
		pool_->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e)
	{
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

std::shared_ptr<UserInfo> MysqlDAO::GetUser(short uid)
{
	auto con = pool_->getConnection();
	try
	{
		if(con == nullptr)
		{
			pool_->returnConnection(std::move(con));
			return nullptr;
		}
		std::unique_ptr<sql::PreparedStatement> statement(
			con->prepareStatement("SELECT * FROM users WHERE uid = ?")
		);
		statement->setInt(1, uid);
		std::unique_ptr<sql::ResultSet> result(statement->executeQuery());
		std::shared_ptr<UserInfo> user_info = nullptr;
		while(result->next())
		{
			user_info.reset(new UserInfo);
			user_info->name = result->getString("name");
			user_info->password = result->getString("password");
			user_info->back = result->getString("back");
			user_info->desc = result->getString("desc");
			user_info->email = result->getString("email");
			user_info->icon = result->getString("icon");
			user_info->nick = result->getString("nick");
			user_info->uid = result->getInt("uid");
			user_info->sex = result->getInt("sex");
			break;
		}
		pool_->returnConnection(std::move(con));
		return user_info;
	}
	catch (sql::SQLException& e)
	{
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}
