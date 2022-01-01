#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H

#include "Const.h"
namespace TFTP {

	class Log_Output {
		//日志文件
		static ofstream FilePoint;    //输出流
		static char timeBuf[100];        //获取时间之后得到的字符串
		Log_Output();   //初始化输出流的地址，清空字节流
		~Log_Output();   //关闭输出流
	public:
		static ostringstream Log_Msg;//输出字节流
		static void logNewLine();   //记录空行到Log_Msg
		static void GetNowTime();   //获得当前的时间到timeBuf数组中
		static void ClearLogMsg();   //清空Log_Msg和timeBuf
		static const char* FindOutput_Msg(int Infor_Type);          //找到当前type对应的字符串地址 [1-5]  或者 0 未定义

		//输出函数当Output_Msg==NULL的时候输出字节流
		static void OutputtoLog(const int Infor_Type,const char* Output_Msg);  //记录日志并输出到控制台
		static void OutputtoCerr(const int Infor_Type,const char* Output_Msg);   //输出到控制台
		static void OutputtoBoth(const int Infor_Type,const char* Output_Msg);   //两个都输出
	};
}

#endif
