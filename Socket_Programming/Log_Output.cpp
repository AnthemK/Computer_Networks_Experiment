#include "Log_Output.h"

namespace TFTP {
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

	void Log_Output::logNewLine() { FilePoint << endl; }

	void Log_Output::GetNowTime() {
		time_t t = time(nullptr);	//Ê±¼ä´Á
		strftime(timeBuf, sizeof(timeBuf), "[%Y-%m-%d %X]", localtime(&t));
	}

	void Log_Output::ClearLogMsg() {
		Log_Msg.str("");
		Log_Msg.clear();
	}

	const char* FindOutput_Msg(int Infor_Type) {
		if (Infor_Type>0&& Infor_Type<5) return Log_Output_Title[Infor_Type];
		else return Log_Output_Title[0];
	}

	void Log_Output::OutputtoLog(const char* Output_Msg) {
		GetNowTime();
		FilePoint << timeBuf << '\n' << Output_Msg <<'\n' << Log_Msg.str();
		ClearLogMsg();
	}

	void Log_Output::OutputtoCerr(const char* Output_Msg) {
		GetNowTime();
		cerr << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		ClearLogMsg();
	}
	void Log_Output::OutputtoBoth(const char* Output_Msg) {
		GetNowTime();
		FilePoint << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		cerr << timeBuf << '\n' << Output_Msg << '\n' << Log_Msg.str();
		ClearLogMsg();
	}
}
