#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H

#include "Client_Const.h"

namespace TFTP {

	class Log_Output {
		//日志文件
		static ofstream FilePoint;   //输出流
		static char timeBuf[50];        //时间函数
		Log_Output();
		~Log_Output();
	public:
		static ostringstream Log_Msg;//输出字节流
		static void logNewLine();   //记录空行
		static void GetNowTime();   //获得当前的时间导timeBuf数组中
		static void ClearLogMsg();   //清空Log_Msg
		const char* FindOutput_Msg(int Infor_Type);          //找到当前type对应的字符串地址

		static void OutputtoLog(const char* Output_Msg);  //记录日志并输出到控制台
		static void OutputtoCerr(const char* Output_Msg);   //输出到控制台
		static void OutputtoBoth(const char* Output_Msg);   //两个都输出
	};


}

#endif
