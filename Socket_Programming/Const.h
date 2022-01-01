#ifndef _CONST_H
#define _CONST_H
//#pragma once   //Ҳ������ֻ����һ�ε����ã�����������Ҫ��һЩ


#define _CRT_SECURE_NO_WARNINGS
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
#include <assert.h>
#include <sys/timeb.h>
#include <ctime>
#include <ratio>
#include <chrono>
#pragma comment(lib, "ws2_32.lib")
#define outt(x)  cout<<#x<<" = "<<x<<"    "
#define hh puts("")

#if defined(WIN32)
# define  TIMEB    _timeb
# define  ftime    _ftime
#else
#define TIMEB timeb
#endif
//��ȡ���뼶ʱ���õ� �����Ѿ�����

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
	using ll = long long;

	constexpr uint DefIp = 0x0100007f;	//Ĭ��IP(127.0.0.1)
	constexpr ll DefTimeOut = 200;		//Ĭ�ϳ�ʱʱ��
	constexpr uint DefRetries = 6;		//Ĭ���ش�����
	constexpr uint DefSleepTime = 10;      //Ĭ��û��20ms�鿴һ���׽��ֻ���
	constexpr uint16 DefPort = 69;	//��һ�����ӵķ������˿ں�
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

	constexpr char RQMode[][15] = {"octet" ,"netascii" };	//octet,netasciiģʽ
	constexpr int DefBufSize = 1024;	//Ĭ�ϻ�������С
	constexpr int DataMaxSize = 512;	//������ݴ�С
	//Ĭ�ϴ�����Ϣ
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};
	//���UDPSocket��ʱ���Ĭ�ϴ�����Ϣ
	constexpr char ErrMsg_getUDPSocket[][50] = {
		"�ײ�������ϵͳû��׼���á�","Winsock �汾��Ϣ�Ų�֧�֡�","����ʽ Winsock1.1 �����ڽ����С�",
		"�Ѿ��ﵽ Winsock ʹ���������ޡ�","lpWSAData ����һ����Ч��ָ��"
	};
	//�����־��5��Title
	constexpr char Log_Output_Title[][35] = {
		"Undefined Information:","Error:","Success:","Warning:","Information:"
	};
	//constexpr int ProcessWorkType = 0;      //�������̵Ĺ���ģʽ 0->����̨ģʽ 1->������ģʽ    �ѷ���
	constexpr bool ClearLog = true;      //�Ƿ��ڳ�ʼ��ʱ���Log�ļ�
	extern bool ReadInforFromConfiguration;       //�Ƿ�ֱ�Ӵ������ļ��ж�ȡIP��ַ������ģʽ���ļ����ȵ�  ֻ����Consoleģʽ����Ч����Ϊtrueʱ�����������ʾ��Ϣ
	extern bool showInfo;	//�Ƿ�չʾ����ϸ�ڱ�ʶ
	extern int SktAddrLen;	//sockaddr��С
	extern bool EchoInputPara;   //�Ƿ�����������


	long long GetCurrentmsTime();   //��ȡ��ǰ��ʱ�� ���뼶
}

#endif