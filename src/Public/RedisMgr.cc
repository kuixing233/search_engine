#include "RedisMgr.h"
#include "ConfigMgr.h"
#include <string.h>

RedisMgr::RedisMgr()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
    Connect(host, std::stoi(port));
}

RedisMgr::~RedisMgr()
{
	Close();
}

// 连接Redis服务
bool RedisMgr::Connect(const std::string& host, int port)
{
	connect = redisConnect(host.c_str(), port);
	if (connect != NULL && connect->err)
	{
		std::cout << "connect redis server error " << connect->errstr << std::endl;
		return false;
	}
	return true;
	return true;
}

// 密码认证
bool RedisMgr::Auth(const std::string& password)
{
	_reply = (redisReply*)redisCommand(connect, "AUTH %s", password.c_str());
	if (_reply->type == REDIS_REPLY_ERROR) {
		std::cout << "Redis认证失败！\n" << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	else {
		std::cout << "Redis认证成功！\n" << std::endl;
		freeReplyObject(_reply);
		return true;
	}
	return true;
}

// 获取key对应的value
bool RedisMgr::Get(const std::string& key, std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());
	if (_reply == NULL)
	{
		std::cout << "[ GET " << key << " ] failed" << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	if (_reply->type != REDIS_REPLY_STRING)
	{
		std::cout << "[ GET " << key << " ] failed" << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	value = _reply->str;
	freeReplyObject(_reply);

	std::cout << "Succeed to execute command [ GET " << key << " ]" << std::endl;
	return true;
}

// 设置key和value
bool RedisMgr::Set(const std::string& key, const std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());
	if (_reply == NULL)
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	if (!(_reply->type == REDIS_REPLY_STATUS && (strcmp(_reply->str, "OK") == 0 || strcmp(_reply->str, "ok") == 0)))
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	return true;
}

// 左侧push
bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
	if (_reply == NULL)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	if (this->_reply->type != REDIS_REPLY_INTEGER || this->_reply->integer <= 0)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	return true;
}

// 左侧pop
bool RedisMgr::LPop(const std::string& key, std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "LPOP %s", key.c_str());
	if (_reply == NULL || _reply->type == REDIS_REPLY_NIL)
	{
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	value = _reply->str;
	freeReplyObject(_reply);
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	return true;
}

// 右侧push
bool RedisMgr::RPush(const std::string& key, const std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
	if (_reply == NULL)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}
	if (this->_reply->type != REDIS_REPLY_INTEGER || this->_reply->integer <= 0)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	return true;
}

// 右侧pop
bool RedisMgr::RPop(const std::string& key, std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "RPOP %s", key.c_str());
	if (_reply == NULL || _reply->type == REDIS_REPLY_NIL)
	{
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	value = _reply->str;
	freeReplyObject(_reply);
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	return true;
}

// HSet操作
bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (_reply == NULL || _reply->type != REDIS_REPLY_INTEGER)
	{
		std::cout << "Execut command [ HSET " << key << hkey << value << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Execut command [ HSET " << key << hkey << value << " ] success ! " << std::endl;
	return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
	if (connect == nullptr) {
		return false;
	}
	const char* argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;
	this->_reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);
	if (_reply == nullptr || _reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(this->_reply);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(this->_reply);
	return true;
}

// HGet操作
std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
	if (connect == nullptr) {
		return std::string();
	}
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();
	this->_reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
	if (this->_reply == nullptr || this->_reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(this->_reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		return "";
	}

	std::string value = this->_reply->str;
	freeReplyObject(this->_reply);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

// Del操作
bool RedisMgr::Del(const std::string& key)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());
	if (_reply == NULL || _reply->type != REDIS_REPLY_INTEGER)
	{
		std::cout << "Execut command [ DEL " << key << " ] failure ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	return true;
}

// 判断键值是否存在
bool RedisMgr::ExistsKey(const std::string& key)
{
	if (connect == nullptr) {
		return false;
	}
	_reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());
	if (_reply == NULL || _reply->type != REDIS_REPLY_INTEGER || _reply->integer == 0)
	{
		std::cout << "Not Found [ Key " << key << " ] ! " << std::endl;
		freeReplyObject(_reply);
		return false;
	}

	freeReplyObject(_reply);
	std::cout << "Found [ Key " << key << " ] exists ! " << std::endl;
	return true;
}

// 关闭Redis连接
void RedisMgr::Close()
{
	// connect->Close();
}
