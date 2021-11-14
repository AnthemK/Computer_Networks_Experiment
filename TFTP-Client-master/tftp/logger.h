#pragma once
#include "const.h"

namespace tftp {
	constexpr char LogERR[] = " <ERR>  ";
	constexpr char LogINFO[] = " <INFO> ";
	constexpr char LogWAIN[] = " <WARN> ";
	constexpr char LogNULL[] = "        ";

	//日志
	class Logger {
		//日志文件
		static ofstream logFile;
		static char timeBuf[40];
	public:
		//输出字节流
		static ostringstream ost;
		//记录空行
		static void logNewLine() { logFile << endl; }
		//记录日志并输出到控制台
		static void logshow(const char* label);
		//输出到控制台
		static void show(const char* label);
		//记录日志
		static void log(const char* label);
	};
}
