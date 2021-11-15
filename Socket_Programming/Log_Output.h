#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H

#include "Client_Const.h"

namespace TFTP {

	class Log_Output {
		//��־�ļ�
		static ofstream FilePoint;   //�����
		static char timeBuf[50];        //ʱ�亯��
		Log_Output();
		~Log_Output();
	public:
		static ostringstream Log_Msg;//����ֽ���
		static void logNewLine();   //��¼����
		static void GetNowTime();   //��õ�ǰ��ʱ�䵼timeBuf������
		static void ClearLogMsg();   //���Log_Msg
		const char* FindOutput_Msg(int Infor_Type);          //�ҵ���ǰtype��Ӧ���ַ�����ַ

		static void OutputtoLog(const char* Output_Msg);  //��¼��־�����������̨
		static void OutputtoCerr(const char* Output_Msg);   //���������̨
		static void OutputtoBoth(const char* Output_Msg);   //���������
	};


}

#endif
