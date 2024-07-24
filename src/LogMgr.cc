#include "LogMgr.h"

#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>

#include <iostream>
using std::cout;
using std::endl;
 
LogMgr::LogMgr()
	: _mycat(log4cpp::Category::getRoot().getInstance("MyCategory"))
{
	using namespace log4cpp;
	PatternLayout * ptn1 = new PatternLayout();
	ptn1->setConversionPattern("%d %c [%p] %m%n");

	PatternLayout * ptn2 = new PatternLayout();
	ptn2->setConversionPattern("%d %c [%p] %m%n");

	OstreamAppender * ostreamAppender = new OstreamAppender("OstreamAppender", &cout);
	ostreamAppender->setLayout(ptn1);

	FileAppender * fileAppender = new FileAppender("FileAppender", "../log/MyLog.log");
	fileAppender->setLayout(ptn2);

	_mycat.addAppender(ostreamAppender);
	_mycat.addAppender(fileAppender);
	_mycat.setPriority(log4cpp::Priority::DEBUG);
	cout << "LogMgr()" << endl;
}

LogMgr::~LogMgr() 
{
	cout << "~LogMgr()" << endl;
}

void LogMgr::warn(const char * msg)
{
	_mycat.warn(msg);
}
void LogMgr::error(const char * msg)
{
	_mycat.error(msg);
}
void LogMgr::info(const char * msg)
{
	_mycat.info(msg);
}
void LogMgr::debug(const char * msg)
{
	_mycat.debug(msg);
}

void LogMgr::setPriority(Priority p)
{
	switch(p)
	{
	case WARN:
		_mycat.setPriority(log4cpp::Priority::WARN);
		break;
	case ERROR:
		_mycat.setPriority(log4cpp::Priority::ERROR);
		break;
	case INFO:
		_mycat.setPriority(log4cpp::Priority::INFO);
		break;
	case DEBUG:
		_mycat.setPriority(log4cpp::Priority::DEBUG);
		break;
	}
}