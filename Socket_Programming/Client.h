#ifndef _Client_H
#define _Client_H

#include "Const.h"
#include "Log_Output.h"
#include "General_Class.h"
#include "Packet.h"

namespace TFTP {

	class Client_Class {
		UDPInfor Connection_Infor;       //���ӵ�ͨ����Ϣ
		Packet SendPkt, ReceivePkt;      //���Ͱ��ͽ��ܰ�
		int File_DataMode;           //�ļ��Ĵ������� 0 -��octet 1-�� netascii
		int err;          //�����н����͵Ĵ�����Ϣ����
		uint16 Now_BlkNO;        //���ڴ���İ��� 0-65536

		int SendPacket();       //����һ�����ݰ� ����SendPkt�� ������Ϣ��Connection_Infor��
		int RecivePacket();     //����һ�����ݰ� ����RecivePkt�� ������Ϣ��Connection_Infor��,�׽�����Ϣ����Recived_addr��
		int AssemblyRQPacket();    //�齨һ��R/Q�����ݰ�      ע��˴���Ҫ�õ��ı����������ʼ��
		int Parse_ErrorPackets();           //����һ��error�����������
		int BestEffort_Send();         //�ṩһ��������Ϊ�ķ��ͣ���ʱ�ش�����ֻ���data����RW�����   ��SendPkt���͵�Connection_Infor.addr��
		int WaitingForDATA();         //�����������ȴ�һ�����ݰ�
		int Make_Connection();         //����һ������
		int Upload_File();         
		//����һ������  
		//��ʱĬ��Ҫ�ϴ����ļ�������Connection_Infor.FilePath��  ���ص��ļ��Ѿ��򿪲���ָ��ΪConnection_Infor.Local_FilePointer
		//File_DataMode��FunctionType�ѱ�ȷ�� 
		int Download_File();       //����һ������ ��ʱĬ��Ҫ���ص��ļ�������Connection_Infor.FilePath��  ���ص��ļ��Ѿ��򿪲���ָ��ΪConnection_Infor.Local_FilePointer
	};

}





#endif