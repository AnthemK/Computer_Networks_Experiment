#ifndef _LOG_OUTPUT_H
#define _LOG_OUTPUT_H

#include "Const.h"
namespace TFTP {

	class Log_Output {
		//��־�ļ�
		static ofstream FilePoint;    //�����
		static char timeBuf[100];        //��ȡʱ��֮��õ����ַ���
		Log_Output();   //��ʼ��������ĵ�ַ������ֽ���
		~Log_Output();   //�ر������
	public:
		static ostringstream Log_Msg;//����ֽ���
		static void logNewLine();   //��¼���е�Log_Msg
		static void GetNowTime();   //��õ�ǰ��ʱ�䵽timeBuf������
		static void ClearLogMsg();   //���Log_Msg��timeBuf
		static const char* FindOutput_Msg(int Infor_Type);          //�ҵ���ǰtype��Ӧ���ַ�����ַ [1-5]  ���� 0 δ����

		//���������Output_Msg==NULL��ʱ������ֽ���
		static void OutputtoLog(const int Infor_Type,const char* Output_Msg);  //��¼��־�����������̨
		static void OutputtoCerr(const int Infor_Type,const char* Output_Msg);   //���������̨
		static void OutputtoBoth(const int Infor_Type,const char* Output_Msg);   //���������
	};
}

#endif
