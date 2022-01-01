#ifndef _CONST_H
#define _CONST_H
//#pragma once   //也可以起到只编译一次的作用，但是适用性要差一些


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
//获取毫秒级时间用的 现在已经废弃

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

	constexpr uint DefIp = 0x0100007f;	//默认IP(127.0.0.1)
	constexpr ll DefTimeOut = 200;		//默认超时时间
	constexpr uint DefRetries = 6;		//默认重传次数
	constexpr uint DefSleepTime = 10;      //默认没过20ms查看一次套接字缓存
	constexpr uint16 DefPort = 69;	//第一次连接的服务器端口号
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

	constexpr char RQMode[][15] = {"octet" ,"netascii" };	//octet,netascii模式
	constexpr int DefBufSize = 1024;	//默认缓冲区大小
	constexpr int DataMaxSize = 512;	//最大数据大小
	//默认错误信息
	constexpr char ErrMsg[][34] = {
		"Undefined error code","File not found","Access violation",
		"Disk full or allocation exceeded","Illegal TFTP operation",
		"Unknown transfer ID","File already exists","No such user"
	};
	//获得UDPSocket的时候的默认错误信息
	constexpr char ErrMsg_getUDPSocket[][50] = {
		"底层网络子系统没有准备好。","Winsock 版本信息号不支持。","阻塞式 Winsock1.1 存在于进程中。",
		"已经达到 Winsock 使用量的上限。","lpWSAData 不是一个有效的指针"
	};
	//输出日志的5种Title
	constexpr char Log_Output_Title[][35] = {
		"Undefined Information:","Error:","Success:","Warning:","Information:"
	};
	//constexpr int ProcessWorkType = 0;      //整个进程的工作模式 0->控制台模式 1->命令行模式    已废弃
	constexpr bool ClearLog = true;      //是否在初始化时清空Log文件
	extern bool ReadInforFromConfiguration;       //是否直接从配置文件中读取IP地址，下载模式、文件名等等  只有在Console模式才生效，当为true时，不会输出提示信息
	extern bool showInfo;	//是否展示传输细节标识
	extern int SktAddrLen;	//sockaddr大小
	extern bool EchoInputPara;   //是否回显输入参数


	long long GetCurrentmsTime();   //获取当前的时间 毫秒级
}

#endif