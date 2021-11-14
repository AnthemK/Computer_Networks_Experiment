#pragma once
#include "const.h"

namespace tftp {
	constexpr char LogERR[] = " <ERR>  ";
	constexpr char LogINFO[] = " <INFO> ";
	constexpr char LogWAIN[] = " <WARN> ";
	constexpr char LogNULL[] = "        ";

	//��־
	class Logger {
		//��־�ļ�
		static ofstream logFile;
		static char timeBuf[40];
	public:
		//����ֽ���
		static ostringstream ost;
		//��¼����
		static void logNewLine() { logFile << endl; }
		//��¼��־�����������̨
		static void logshow(const char* label);
		//���������̨
		static void show(const char* label);
		//��¼��־
		static void log(const char* label);
	};
}
