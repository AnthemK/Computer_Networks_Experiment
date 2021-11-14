#include "logger.h"

namespace tftp {
	ofstream Logger::logFile("log.txt", ofstream::app);	//日志文件
	ostringstream Logger::ost;	//输出字节流
	char Logger::timeBuf[40];

	/// <summary>
	/// 记录日志并输出到控制台
	/// </summary>
	/// <param name="label">标签</param>
	void Logger::logshow(const char* label) {
		time_t t = time(nullptr);	//时间戳
		strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %X", localtime(&t));
		logFile << '[' << timeBuf << ']'
			<< label << ost.str();
		cerr << label << ost.str();
		ost.str("");	//清空字节流
		ost.clear();
	}

	/// <summary>
	/// 输出到控制台
	/// </summary>
	/// <param name="label">标签</param>
	void Logger::show(const char* label) {
		cerr << label << ost.str();
		ost.str("");
		ost.clear();
	}

	/// <summary>
	/// 记录日志
	/// </summary>
	/// <param name="label">标签</param>
	void Logger::log(const char* label) {
		time_t t = time(nullptr);
		logFile << '[' << ctime(&t) << ']'
			<< label << ost.str();
	}

}

