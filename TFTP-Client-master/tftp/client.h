#pragma once
#include "const.h"
#include "udpaddr.h"
#include "connection.h"
#include "logger.h"


namespace tftp {
	//tftp客户端
	class Client {
		//服务器地址
		UDPAddr svrAddr;
		//初始化Winsock标志
		bool initWinSkt = false;

		//建立tftp连接
		SOCKET openConn();
		//关闭tftp连接
		void closeConn(SOCKET skt) { closesocket(skt); }
		//处理文件路径
		void handleFilePath(const string& filepath, char* filename) const;
	public:
		Client(byte a, byte b, byte c, byte d, uint16 p) : svrAddr(a,b,c,d,p) {}
		Client(const char* ip) : svrAddr(ip, DefSvrPort) {}
		//启动Winsock API
		bool startWinSkt();
		//关闭Winsock API
		void cleanWinSkt() { WSACleanup(), initWinSkt = false; }
		//发送文件
		bool sendFile(const string& filepath, const char* mode);
		//接收文件
		bool recvFile(const string& filepath, const char* mode);
	};

}


