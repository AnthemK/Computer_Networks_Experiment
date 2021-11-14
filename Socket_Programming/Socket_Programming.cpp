// Socket_Programming.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//#include "stdafx.h"
#include "Client_Const.h"

using namespace std;
using namespace TFTP;
const int maxn = 100000;

//创建一个数据包socket
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


int AssemblyACKPack(char* aimstr,short* ACKBlockNo)
{
	char ack[4];
	ack[0] = 0x00;
	ack[1] = 0x04;
	memcpy(ack + 2, ACKBlockNo, 2);
	return true;
}

char InputStr[maxn],SendStr[maxn];
int err, datalen;  //err存储函数返回信息， datalen存储数据长度
int main()
{
	SOCKET MySOC = getUDPSocket();   //初始化获取本地的的socket
WrongIP:
	scanf_s("%s", InputStr, DefBufSize);
	sockaddr_in addr = getSockAddr_in(InputStr, DefSvrPort);
	if (addr.sin_addr.S_un.S_addr == INADDR_NONE) { puts("Illegal IP Address"); goto WrongIP; }
	PrintIP(addr.sin_addr.S_un.S_addr);


WrongFIle:
	scanf_s("%s", InputStr, DefBufSize);
	datalen = AssemblyRequestDownloadPack(SendStr, InputStr, 1);
	err = sendto(MySOC, SendStr, datalen, 0, (sockaddr*)&addr, sizeof(addr));
	SendStr[0] = 0;
	if (err != datalen)
	{
		cout << "sendto failed" << endl;
		goto WrongFIle;
	}

	char* FileName;    //获取文件名
	FILE* fp;    //文件指针
	for (FileName = InputStr + strlen(InputStr); FileName > InputStr && (*FileName) != '\\'; FileName--);
	if (*FileName == '\\')err = fopen_s(&fp, FileName + 1, "wb");
	else err = fopen_s(&fp, FileName, "wb");
	if (err)
	{
		cout << "File open failed!" << endl;
		goto WrongFIle;
	}

	while (true)
	{
		char buf[1024];
		sockaddr_in server;
		int len = sizeof(server);
		err = recvfrom(MySOC, buf, 1024, 0, (sockaddr*)&server, &len);
		//发送请求下载到服务器69端口后，服务端会随机找一个端口来发送数据给客户端，客户端返回确认报文时，不能再向69端口发送ACK，而是向该端口发送ACK，所以需要保存新的地址信息server，用该地址信息来进行数据传输
		if (err > 0)
		{
			short flag;
			memcpy(&flag, buf, 2);
			flag = ntohs(flag); //网络字节序转换为主机字节序，很重要
			if (flag == 3)
			{
				UINT16 a;
					short no;
				memcpy(&no, buf + 2, 2);
				fwrite(buf + 4, err - 4, 1, fp);
				if (err < 512)
				{
					std::cout << "All Package Download Finished!" << std::endl;
					break;
				}
				std::cout <<"Block "<<no << " Download Finished!" << std::endl;
				//发送确认报文
				AssemblyACKPack(SendStr,&no);
				err = sendto(MySOC, SendStr, 4, 0, (sockaddr*)&server, sizeof(server));
				SendStr[0] = 0;
				if (err != 4)
				{
					std::cout << "ACK Error" << WSAGetLastError() << std::endl;
					break;
				}
				else std::cout << "ACK Success" << std::endl;
			} else if (flag == 5)
			{
				//服务端返回了错误信息
				short errorcode;
				memcpy(&errorcode, buf + 2, 2);
				errorcode = ntohs(errorcode);
				InputStr[0] = 0;
				int iter = 0;
				while ((*buf + iter + 4) != 0)
				{
					memcpy(InputStr + iter, buf + iter + 4, 1);
					++iter;
				}
				std::cout << "Error" << errorcode << " :  " << InputStr << std::endl;
				InputStr[0] = 0;
				break;
			}
			else {
				std::cout << "Error Operation Code"<< std::endl;
				break;
			}
		}
	}
	fclose(fp);
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