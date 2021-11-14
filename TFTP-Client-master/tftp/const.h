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

	constexpr uint DefIp = 0x0100007f;	//默认IP
	constexpr uint DefTimeOut = 2;		//默认超时时间
	constexpr uint DefRetries = 5;		//默认重传此时
	constexpr uint16 DefSvrPort = 69;	//默认服务器端口号
	constexpr uint16 OpRRQ = 1;			//读请求操作码
	constexpr uint16 OpWRQ = 2;			//写请求操作码
	constexpr uint16 OpDATA = 3;		//数据操作码
	constexpr uint16 OpACK = 4;			//确认操作码
	constexpr uint16 OpERROR = 5;		//错误操作码
	constexpr char ModeAscii[] = "netascii";	//文本模式
	constexpr char ModeBin[] = "octet";	//二进制模式
	constexpr int DefBufSize = 1024;	//默认缓冲区大小
	constexpr int DataMaxSize = 512;	//最大数据大小
	//默认错误信息
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};

	extern bool showInfo;	//展示传输细节标识
	extern int SktAddrLen;	//sockaddr大小
}
