#ifndef __KX_REDISMGR_H__
#define __KX_REDISMGR_H__

/**
 * 类RedisMgr：实际执行Redis命令的类
 * 类RedisConPool：封装Redis连接池
 */

#include "Singleton.h"
#include <hiredis/hiredis.h>

/*
void TestRedis() {
	//连接redis 需要启动才可以进行连接
	//redis默认监听端口为6387 可以再配置文件中修改
	redisContext* c = redisConnect("127.0.0.1", 6380);
	if (c->err)
	{
		printf("Connect to redisServer faile:%s\n", c->errstr);
		redisFree(c);        return;
	}
	printf("Connect to redisServer Success\n");

	std::string redis_password = "123";
	redisReply* r = (redisReply*)redisCommand(c, "AUTH %s", redis_password);
	if (r->type == REDIS_REPLY_ERROR) {
		printf("Redis认证失败！\n");
	}
	else {
		printf("Redis认证成功！\n");
	}

	//为redis设置key
	const char* command1 = "set stest1 value1";

	//执行redis命令行
	r = (redisReply*)redisCommand(c, command1);

	//如果返回NULL则说明执行失败
	if (NULL == r)
	{
		printf("Execut command1 failure\n");
		redisFree(c);        return;
	}

	//如果执行失败则释放连接
	if (!(r->type == REDIS_REPLY_STATUS && (strcmp(r->str, "OK") == 0 || strcmp(r->str, "ok") == 0)))
	{
		printf("Failed to execute command[%s]\n", command1);
		freeReplyObject(r);
		redisFree(c);        return;
	}

	//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);

	const char* command2 = "strlen stest1";
	r = (redisReply*)redisCommand(c, command2);

	//如果返回类型不是整形 则释放连接
	if (r->type != REDIS_REPLY_INTEGER)
	{
		printf("Failed to execute command[%s]\n", command2);
		freeReplyObject(r);
		redisFree(c);        return;
	}

	//获取字符串长度
	int length = r->integer;
	freeReplyObject(r);
	printf("The length of 'stest1' is %d.\n", length);
	printf("Succeed to execute command[%s]\n", command2);

	//获取redis键值对信息
	const char* command3 = "get stest1";
	r = (redisReply*)redisCommand(c, command3);
	if (r->type != REDIS_REPLY_STRING)
	{
		printf("Failed to execute command[%s]\n", command3);
		freeReplyObject(r);
		redisFree(c);        return;
	}
	printf("The value of 'stest1' is %s\n", r->str);
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command3);

	const char* command4 = "get stest2";
	r = (redisReply*)redisCommand(c, command4);
	if (r->type != REDIS_REPLY_NIL)
	{
		printf("Failed to execute command[%s]\n", command4);
		freeReplyObject(r);
		redisFree(c);        return;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command4);

	//释放连接资源
	redisFree(c);
}
*/


// Redis操作类
class RedisMgr
	: public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	// 连接Redis服务
	bool Connect(const std::string& host, int port);
	// 密码认证
	bool Auth(const std::string& password);
	// 获取key对应的value
	bool Get(const std::string& key, std::string& value);
	// 设置key和value
	bool Set(const std::string& key, const std::string& value);
	// 左侧push
	bool LPush(const std::string& key, const std::string& value);
	// 左侧pop
	bool LPop(const std::string& key, std::string& value);
	// 右侧push
	bool RPush(const std::string& key, const std::string& value);
	// 右侧pop
	bool RPop(const std::string& key, std::string& value);
	// HSet操作
	bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
	// HGet操作
	std::string HGet(const std::string& key, const std::string& hkey);
	// Del操作
	bool Del(const std::string& key);
	// 判断键值是否存在
	bool ExistsKey(const std::string& key);
	// 关闭Redis连接
	void Close();

private:
	RedisMgr();
	
	redisContext* connect;
	redisReply* _reply;
};

#endif