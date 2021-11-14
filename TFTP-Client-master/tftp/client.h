#pragma once
#include "const.h"
#include "udpaddr.h"
#include "connection.h"
#include "logger.h"


namespace tftp {
	//tftp�ͻ���
	class Client {
		//��������ַ
		UDPAddr svrAddr;
		//��ʼ��Winsock��־
		bool initWinSkt = false;

		//����tftp����
		SOCKET openConn();
		//�ر�tftp����
		void closeConn(SOCKET skt) { closesocket(skt); }
		//�����ļ�·��
		void handleFilePath(const string& filepath, char* filename) const;
	public:
		Client(byte a, byte b, byte c, byte d, uint16 p) : svrAddr(a,b,c,d,p) {}
		Client(const char* ip) : svrAddr(ip, DefSvrPort) {}
		//����Winsock API
		bool startWinSkt();
		//�ر�Winsock API
		void cleanWinSkt() { WSACleanup(), initWinSkt = false; }
		//�����ļ�
		bool sendFile(const string& filepath, const char* mode);
		//�����ļ�
		bool recvFile(const string& filepath, const char* mode);
	};

}


