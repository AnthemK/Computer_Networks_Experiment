#ifndef _CLIENT_CONST_H
#define _CLIENT_CONST_H
//#pragma once   //Ҳ������ֻ����һ�ε����ã�����������Ҫ��һЩ


//#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace TFTP{
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::string;
	using std::ifstream;
	using std::ofstream;
	using std::ostringstream;
	using byte = unsigned char;
	using uint16 = UINT16;
	using uint = unsigned int;
	using uint64 = unsigned long long;

	constexpr uint DefIp = 0x0100007f;	//Ĭ��IP(127.0.0.1)�����޸�
	constexpr uint DefTimeOut = 2;		//Ĭ�ϳ�ʱʱ��
	constexpr uint DefRetries = 5;		//Ĭ���ش�����
	constexpr uint16 DefSvrPort = 69;	//��һ�����ӵķ������˿ں�
	constexpr uint16 OpRRQ = 1;			//�����������
	constexpr uint16 OpWRQ = 2;			//д���������
	constexpr uint16 OpDATA = 3;		//���ݲ�����
	constexpr uint16 OpACK = 4;			//ȷ�ϲ�����
	constexpr uint16 OpERROR = 5;		//���������
	constexpr uint16 LogUndefinedCode = 0;        //�����־ʱδ������Ϣ�Ŀ�ͷ��ʶ
	constexpr uint16 LogErrCode = 1;        //�����־ʱ������Ϣ�Ŀ�ͷ��ʶ
	constexpr uint16 LogSuccessCode = 2;     //�����־ʱ�ɹ���Ϣ�Ŀ�ͷ��ʶ
	constexpr uint16 LogWarnCode = 3;         //�����־ʱ������Ϣ�Ŀ�ͷ��ʶ
	constexpr uint16 LogInforCode = 4;         //�����־ʱ��Ϣ�Ŀ�ͷ��ʶ

	constexpr char RQMode[][10] = {"octet" ,"netascii" };	//octet,netasciiģʽ
	constexpr int DefBufSize = 1024;	//Ĭ�ϻ�������С
	constexpr int DataMaxSize = 512;	//������ݴ�С
	//Ĭ�ϴ�����Ϣ
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};
	//���UDPSocket��ʱ���Ĥ�˴�����Ϣ
	constexpr char ErrMsg_getUDPSocket[][50] = {
		"�ײ�������ϵͳû��׼���á�","Winsock �汾��Ϣ�Ų�֧�֡�","����ʽ Winsock1.1 �����ڽ����С�",
		"�Ѿ��ﵽ Winsock ʹ���������ޡ�","lpWSAData ����һ����Ч��ָ��"
	};
	constexpr char Log_Output_Title[][35] = {
		"Undefined Information:","Error:","Success:","Warning:","Information:"
	};
	extern bool showInfo;	//չʾ����ϸ�ڱ�ʶ
	extern int SktAddrLen;	//sockaddr��С
}

#endif