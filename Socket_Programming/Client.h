#ifndef _Client_H
#define _Client_H

#include "Const.h"
#include "Log_Output.h"
#include "General_Class.h"
#include "Packet.h"

namespace TFTP {

	class Client_Class {
		UDPInfor Connection_Infor;       //���ӵ�ͨ����Ϣ
		Packet SendPkt, RecivePkt;      //���Ͱ��ͽ��ܰ�
		int File_DataMode;           //�ļ��Ĵ������� 0 -��octet 1-�� netascii
		int err;          //�����н����͵Ĵ�����Ϣ����

		int SendPacket();       //����һ�����ݰ� ����SendPkt�� ������Ϣ��Connection_Infor��
		int RecivePacket();     //����һ�����ݰ� ����RecivePkt�� ������Ϣ��Connection_Infor��
		int AssemblyRQPacket();    //�齨һ��R/Q�����ݰ�   ע��˴���Ҫ�õ��ı���
		int Parse_ErrorPackets(int Now_Blkno);           //����һ��error�����������
		int BestEffort_Send();         //�ṩһ��������Ϊ�ķ��ͣ���ʱ�ش�����ֻ���data��
		int Make_Connection();         //����һ������
	};


}





#endif