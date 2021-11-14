#include "logger.h"

namespace tftp {
	ofstream Logger::logFile("log.txt", ofstream::app);	//��־�ļ�
	ostringstream Logger::ost;	//����ֽ���
	char Logger::timeBuf[40];

	/// <summary>
	/// ��¼��־�����������̨
	/// </summary>
	/// <param name="label">��ǩ</param>
	void Logger::logshow(const char* label) {
		time_t t = time(nullptr);	//ʱ���
		strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %X", localtime(&t));
		logFile << '[' << timeBuf << ']'
			<< label << ost.str();
		cerr << label << ost.str();
		ost.str("");	//����ֽ���
		ost.clear();
	}

	/// <summary>
	/// ���������̨
	/// </summary>
	/// <param name="label">��ǩ</param>
	void Logger::show(const char* label) {
		cerr << label << ost.str();
		ost.str("");
		ost.clear();
	}

	/// <summary>
	/// ��¼��־
	/// </summary>
	/// <param name="label">��ǩ</param>
	void Logger::log(const char* label) {
		time_t t = time(nullptr);
		logFile << '[' << ctime(&t) << ']'
			<< label << ost.str();
	}

}

