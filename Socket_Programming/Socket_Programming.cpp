// Socket_Programming.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <cstdlib>
#pragma comment(lib,"ws2_32.lib")	
#define  _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;
const int maxn = 10000;

//创建一个数据包socket
int getUDPSocket()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA lpData;
	int err = WSAStartup(ver, &lpData);
	if (err != 0) return -1;
	switch (err) 
	{
		case 0: break;
		case WSASYSNOTREADY: {
			puts("底层网络子系统没有准备好。");
			return -1;
			}
		case WSAVERNOTSUPPORTED: {
			puts("Winsock 版本信息号不支持。");
			return -1;
		}
		case WSAEINPROGRESS: {
			puts("阻塞式 Winsock1.1 存在于进程中。");
			return -1;
		}
		case WSAEPROCLIM: {
			puts("已经达到 Winsock 使用量的上限。");
			return -1;
		}
		case WSAEFAULT: {
			puts("lpWSAData 不是一个有效的指针");
			return -1;
		}
	}
	int udpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);   //使用UDP协议
	if (udpsocket == INVALID_SOCKET) return -2;
	//通过WSAGetLastError取得具体的错误代码
	return udpsocket;
}

//获取TCP/IP协议栈下 指定地址信息数据结构
sockaddr_in getSockAddr_in(char* ip, int port)  //ip  "127.0.0.1."
{
	sockaddr_in adddr;
	adddr.sin_family = AF_INET;
	adddr.sin_port = htons(port);
	//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
	inet_pton(AF_INET, ip, (void*)(&adddr.sin_addr.S_un.S_addr));
	return adddr;
}

//输出一个IP地址 ipv4
bool PrintIP(u_long ipval) {
	printf("IP: %d.%d.%d.%d\n", ipval & 0xff, (ipval>>8) & 0xff, (ipval >> 16) & 0xff, (ipval >> 24) & 0xff);
	return true;
}

int main()
{
	char InputIP[10000];
	WrongIP:
	scanf_s("%s", InputIP,15);
	sockaddr_in addr = getSockAddr_in(InputIP, 69);
	if (addr.sin_addr.S_un.S_addr == INADDR_NONE) { puts("Illegal ip address"); goto WrongIP; }
	PrintIP(addr.sin_addr.S_un.S_addr);



}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
//  HUST CSE Computer Networks Experiment