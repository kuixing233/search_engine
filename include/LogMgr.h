#ifndef __KX_LOGMGR_H__
#define __KX_LOGMGR_H__

#include "Singleton.h"

#include <log4cpp/Category.hh>
#include <string>

using std::string;

class LogMgr
	: public Singleton<LogMgr>
{
	friend class Singleton<LogMgr>;
public:
	enum Priority {
		ERROR = 300,
		WARN,
		INFO,
		DEBUG
	};

	~LogMgr();

	template <class... Args>
	void warn(const char * msg, Args... args)
	{
		_mycat.warn(msg, args...);
	}
	template <class... Args>
	void error(const char * msg, Args... args)
	{
		_mycat.error(msg, args...);
	}
	template <class... Args>
	void info(const char * msg, Args... args)
	{
		_mycat.info(msg, args...);
	}

	template <class... Args>
	void debug(const char * msg, Args... args)
	{
		_mycat.debug(msg, args...);
	}

	void warn(const char * msg);
	void error(const char * msg);
	void info(const char * msg);
	void debug(const char * msg);

	void setPriority(Priority p);

private:
	LogMgr();

private:
	log4cpp::Category & _mycat;
};

#define prefix(msg) string("[")\
	.append(__FILE__).append(":")\
	.append(__FUNCTION__).append(":")\
	.append(std::to_string(__LINE__)).append("] ")\
	.append(msg).c_str()

#define LogError(msg, ...) LogMgr::GetInstance()->error(prefix(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...) LogMgr::GetInstance()->warn(prefix(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...) LogMgr::GetInstance()->info(prefix(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) LogMgr::GetInstance()->debug(prefix(msg), ##__VA_ARGS__)

#endif
