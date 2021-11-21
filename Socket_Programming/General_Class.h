#ifndef _GENERAL_CLASS_H
#define _GENERAL_CLASS_H

#include "Const.h"
#include "Log_Output.h"

namespace TFTP {

	int getUDPSocket();  //��ʼ����ȡ���ص�SOCKET
	bool PrintIP(u_long ipval);      //���һ��IP��ַ
	bool PrintPort(uint16 port);    //���һ���˿ںţ��˶˿ں�Ӧ��Ϊ�����ֽ����ʽ
	bool FromsockaddrPrintIPandPort(const sockaddr_in printed_addr);


	//UDP��ַ
	class UDPInfor {
	public:
		sockaddr_in addr;	   //�Է���socketaddr
		SOCKET Local_Socket;    //���ص�socet
		FILE* Local_FilePointer;   //���ش��ļ���ָ��
		int FunctionType;          //��ʾ���ڵĹ������� 0��ʾδ֪  1��ʾ���ļ�  2��ʾд�ļ�
		char FilePath[100];         //�ļ�·���ַ���
		time_t Begin_Time;         //���ӿ�ʼ��ʱ��


		//???????????????????????????????????????
		//�Դ�һ��û�г�ʼ��
		time_t ResendTimer;         //�ش���ʱ��
		int RemainResendNum;       //ʣ����ش�����
		int SuccessBytes;          //�Ѿ�����ɹ����ֽ�����ACK��Żᱻ����


		UDPInfor();
		UDPInfor(const char* ip, uint16 port);   //��һ���ַ�����ʼ��IP��ַ�Լ��˿ں�
		UDPInfor(byte a, byte b, byte c, byte d, uint16 port);  //���ĸ�byte��ʼ��IP ��ַ �Լ��˿ں�
		
		uint16 ChangePort(uint16 NewPort);     //�ı�ip��ַ
		uint16 ChangeIP(const char* ip);      //�ı�˿ں�
		sockaddr* getSktAddr();   //��ȡsockaddr��ָ��
	};
}





#endif 