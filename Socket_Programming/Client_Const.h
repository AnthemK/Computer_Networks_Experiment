#ifndef _CLIENT_CONST_H
#define _CLIENT_CONST_H
//#pragma once   //也可以起到只编译一次的作用，但是适用性要差一些


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

	constexpr uint DefIp = 0x0100007f;	//默认IP(127.0.0.1)，待修改
	constexpr uint DefTimeOut = 2;		//默认超时时间
	constexpr uint DefRetries = 5;		//默认重传次数
	constexpr uint16 DefSvrPort = 69;	//第一次连接的服务器端口号
	constexpr uint16 OpRRQ = 1;			//读请求操作码
	constexpr uint16 OpWRQ = 2;			//写请求操作码
	constexpr uint16 OpDATA = 3;		//数据操作码
	constexpr uint16 OpACK = 4;			//确认操作码
	constexpr uint16 OpERROR = 5;		//错误操作码
	constexpr uint16 LogUndefinedCode = 0;        //输出日志时未定义信息的开头标识
	constexpr uint16 LogErrCode = 1;        //输出日志时错误信息的开头标识
	constexpr uint16 LogSuccessCode = 2;     //输出日志时成功信息的开头标识
	constexpr uint16 LogWarnCode = 3;         //输出日志时警告信息的开头标识
	constexpr uint16 LogInforCode = 4;         //输出日志时信息的开头标识

	constexpr char RQMode[][10] = {"octet" ,"netascii" };	//octet,netascii模式
	constexpr int DefBufSize = 1024;	//默认缓冲区大小
	constexpr int DataMaxSize = 512;	//最大数据大小
	//默认错误信息
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};
	//获得UDPSocket的时候的膜人错误信息
	constexpr char ErrMsg_getUDPSocket[][50] = {
		"底层网络子系统没有准备好。","Winsock 版本信息号不支持。","阻塞式 Winsock1.1 存在于进程中。",
		"已经达到 Winsock 使用量的上限。","lpWSAData 不是一个有效的指针"
	};
	constexpr char Log_Output_Title[][35] = {
		"Undefined Information:","Error:","Success:","Warning:","Information:"
	};
	extern bool showInfo;	//展示传输细节标识
	extern int SktAddrLen;	//sockaddr大小
}

#endif