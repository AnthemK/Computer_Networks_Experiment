#include "General_Class.h"

namespace TFTP {

	int getUDPSocket()
	{
		WORD ver = MAKEWORD(2, 2);
		WSADATA lpData;
		int err = WSAStartup(ver, &lpData);//欲使用的 Windows Sockets API 版本,高字节定义的是次版本号，低字节定义的是主版本号
		if (err != 0) return -1;
		switch (err)
		{
			case 0: break;
			case WSASYSNOTREADY: {
				puts(ErrMsg_getUDPSocket[0]);
				return -1;
			}
			case WSAVERNOTSUPPORTED: {
				puts(ErrMsg_getUDPSocket[1]);
				return -1;
			}
			case WSAEINPROGRESS: {
				puts(ErrMsg_getUDPSocket[2]);
				return -1;
			}
			case WSAEPROCLIM: {
				puts(ErrMsg_getUDPSocket[3]);
				return -1;
			}
			case WSAEFAULT: {
				puts(ErrMsg_getUDPSocket[4]);
				return -1;
			}
		}
		int udpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //使用UDP协议,数据报类型
		if (udpsocket == INVALID_SOCKET) return -2;
		//通过WSAGetLastError取得具体的错误代码
		return udpsocket;
	}

	bool PrintIP(u_long ipval) {
		printf("IP: %d.%d.%d.%d\n", ipval & 0xff, (ipval >> 8) & 0xff, (ipval >> 16) & 0xff, (ipval >> 24) & 0xff);
		return true;
	}


	bool PrintPort(uint16 port) {
		printf("Port: %d\n", ntohs(port));
		return true;
	}

	bool FromsockaddrPrintIPandPort(const sockaddr_in printed_addr) {
		printf("IP:Port  %s : %d\n", inet_ntoa(printed_addr.sin_addr), printed_addr.sin_port);
		return true;
	}

	UDPInfor::UDPInfor() {
		addr.sin_port = htons(DefPort);
		addr.sin_addr.S_un.S_addr = DefIp;
		Local_Socket = getUDPSocket();
		Local_FilePointer = NULL;
		FunctionType = 0;
		FilePath[0] = 0;
		Begin_Time = time(NULL);
	}
	UDPInfor::UDPInfor(const char* ip, uint16 port) {
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
		int err=inet_pton(AF_INET, ip, (void*)(&addr.sin_addr.S_un.S_addr));
		if(err!=1) addr.sin_addr.S_un.S_addr = DefIp;
		Local_Socket = getUDPSocket();
		Local_FilePointer = NULL;
		FunctionType = 0;
		FilePath[0] = 0;
		Begin_Time = time(NULL);
	}
	UDPInfor::UDPInfor(byte a, byte b, byte c, byte d, uint16 port) {
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_un_b.s_b1 = a;
		addr.sin_addr.S_un.S_un_b.s_b2 = b;
		addr.sin_addr.S_un.S_un_b.s_b3 = c;
		addr.sin_addr.S_un.S_un_b.s_b4 = d;
		addr.sin_port = htons(port);
		Local_Socket = getUDPSocket();
		Local_FilePointer = NULL;
		FunctionType = 0;
		FilePath[0] = 0;
		Begin_Time = time(NULL);
	}
	uint16 UDPInfor::ChangePort(uint16 NewPort) {
		addr.sin_port = htons(NewPort);
	}
	uint16 UDPInfor::ChangeIP(const char* ip) {
		int err = inet_pton(AF_INET, ip, (void*)(&addr.sin_addr.S_un.S_addr));
		if (err != 1) addr.sin_addr.S_un.S_addr = DefIp;
	}
	sockaddr* UDPInfor::getSktAddr() { return (sockaddr*)&addr; }
}