#include "Log_Output.h"

namespace TFTP {
	ofstream Log_Output::FilePoint("log.txt", ofstream::app);	//�˴�Ϊ��ʼ��static����   ��־�ļ�
	ostringstream Log_Output::Log_Msg;	//����ֽ���
	char Log_Output::timeBuf[100];
	//��������Ϊ��ʼ��  ����ռ�

	Log_Output::Log_Output() {
		FilePoint.open("log.txt", ofstream::app);
		timeBuf[0] = 0;
		Log_Msg.str("");
		Log_Msg.clear();
	}
	Log_Output::~Log_Output() {
		FilePoint.close();
		timeBuf[0] = 0;
		Log_Msg.str("");
		Log_Msg.clear();
	}

	void Log_Output::logNewLine() { Log_Msg << endl; }

	void Log_Output::GetNowTime() {
		time_t t = time(nullptr);	//ʱ���
		strftime(timeBuf, sizeof(timeBuf), "[%Y-%m-%d %X]", localtime(&t));
	}

	void Log_Output::ClearLogMsg() {
		Log_Msg.str("");
		Log_Msg.clear();
		timeBuf[0] = 0;
	}

	const char* Log_Output::FindOutput_Msg(int Infor_Type) {
		if (Infor_Type>0 && Infor_Type<5) return Log_Output_Title[Infor_Type];
		else return Log_Output_Title[0];
	}

	void Log_Output::OutputtoLog(const int Infor_Type,const char* Output_Msg) {
		GetNowTime();
		if (Output_Msg != NULL) FilePoint << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		else FilePoint << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Log_Msg.str();
		ClearLogMsg();
	}

	void Log_Output::OutputtoCerr(const int Infor_Type,const char* Output_Msg) {
		GetNowTime();
		if (Output_Msg != NULL) cerr << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		else cerr << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Log_Msg.str();
		ClearLogMsg();
	}
	void Log_Output::OutputtoBoth(const int Infor_Type,const char* Output_Msg) {
		GetNowTime();
		if (Output_Msg != NULL) {
			FilePoint << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
			cerr << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		}
		else {
			FilePoint << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Log_Msg.str();
			cerr << FindOutput_Msg(Infor_Type) << '\n' << timeBuf << '\n' << Log_Msg.str();
		}
		ClearLogMsg();
	}
}
