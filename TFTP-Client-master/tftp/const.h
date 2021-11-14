#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")



namespace tftp {
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::string;
	using std::ifstream;
	using std::ofstream;
	using std::ostringstream;
	using byte = unsigned char;
	using uint16 = unsigned short;
	using uint = unsigned;
	using uint64 = unsigned long long;

	constexpr uint DefIp = 0x0100007f;	//Ĭ��IP
	constexpr uint DefTimeOut = 2;		//Ĭ�ϳ�ʱʱ��
	constexpr uint DefRetries = 5;		//Ĭ���ش���ʱ
	constexpr uint16 DefSvrPort = 69;	//Ĭ�Ϸ������˿ں�
	constexpr uint16 OpRRQ = 1;			//�����������
	constexpr uint16 OpWRQ = 2;			//д���������
	constexpr uint16 OpDATA = 3;		//���ݲ�����
	constexpr uint16 OpACK = 4;			//ȷ�ϲ�����
	constexpr uint16 OpERROR = 5;		//���������
	constexpr char ModeAscii[] = "netascii";	//�ı�ģʽ
	constexpr char ModeBin[] = "octet";	//������ģʽ
	constexpr int DefBufSize = 1024;	//Ĭ�ϻ�������С
	constexpr int DataMaxSize = 512;	//������ݴ�С
	//Ĭ�ϴ�����Ϣ
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};

	extern bool showInfo;	//չʾ����ϸ�ڱ�ʶ
	extern int SktAddrLen;	//sockaddr��С
}
