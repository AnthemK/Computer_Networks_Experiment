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
		SOCKET udpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //使用UDP协议,数据报类型
		if (udpsocket == INVALID_SOCKET) return -2;
		//可以通过WSAGetLastError取得具体的错误代码
		u_long imode = 1;
		auto ret = ioctlsocket(udpsocket, FIONBIO, &imode);
		if (ret == SOCKET_ERROR) {
			return -2;
		}
		cout << "Init Complete" << endl;
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
		printf("IP:Port  %s : %d\n", inet_ntoa(printed_addr.sin_addr), ntohs(printed_addr.sin_port));
		return true;
	}

	char* GetFileName(char* FilePath) {
		char* TmpStr;
		for (TmpStr = (FilePath + strlen(FilePath)); TmpStr > FilePath && (*TmpStr) != '\\'; TmpStr--);
		if (*TmpStr == '\\')return TmpStr+1;
		else return TmpStr;
	}


	int CreateFilePointer(char* FilePath, int Open_Type,FILE* &Aimfp) {
		int errr = 0;
		if (Open_Type == 1) {
			errr= fopen_s(&Aimfp, FilePath, "wb");   //下载文件 因此要写
		}
		else if (Open_Type == 2) {
			errr = fopen_s(&Aimfp, FilePath, "rb");   //上载文件 因此要读
		}
		else {
			errr = -1;
		}
		if (errr) {
			Log_Output::OutputtoBoth(1, "File open failed!");
		}
		return errr;
	}

	UDPInfor::UDPInfor() {
		addr.sin_family = AF_INET;
		addr.sin_port = htons(DefPort);
		addr.sin_addr.S_un.S_addr = DefIp;
		Local_Socket = getUDPSocket();
		Local_FilePointer = NULL;
		FunctionType = 0;
		FilePath[0] = 0;
		Begin_Time = time(NULL);
		ResendTimer = Begin_Time;
		RemainResendNum = 0;
		SuccessBytes = 0;
	}
	UDPInfor::UDPInfor(const char* ip, uint16 port) {
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
		int errr=inet_pton(AF_INET, ip, (void*)(&addr.sin_addr.S_un.S_addr));
		if(errr!=1) addr.sin_addr.S_un.S_addr = DefIp;
		Local_Socket = getUDPSocket();
		Local_FilePointer = NULL;
		FunctionType = 0;
		FilePath[0] = 0;
		Begin_Time = time(NULL);
		ResendTimer = Begin_Time;
		RemainResendNum = 0;
		SuccessBytes = 0;
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
		ResendTimer = Begin_Time;
		RemainResendNum = 0;
		SuccessBytes = 0;
	}
	uint16 UDPInfor::ChangePort(uint16 NewPort) {
		return (addr.sin_port = htons(NewPort));
	}
	uint64 UDPInfor::ChangeIP(const char* ip) {
		int errr = inet_pton(AF_INET, ip, (void*)(&addr.sin_addr.S_un.S_addr));
		if (errr != 1) addr.sin_addr.S_un.S_addr = DefIp;
		return addr.sin_addr.S_un.S_addr;
	}
	sockaddr* UDPInfor::getSktAddr() { return (sockaddr*)&addr; }
}