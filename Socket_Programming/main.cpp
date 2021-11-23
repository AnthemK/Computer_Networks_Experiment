// Socket_Programming.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//#include "stdafx.h"
/*#include "Const.h"
#include "Log_Output.h"
#include "Packet.h"
#include "General_Class.h"
*/
#include "Client.h"
using namespace std;
using namespace TFTP;
const int maxn = 10000;

char InputStr[maxn],SendStr[maxn];
int err, datalen;  //err存储函数返回信息， datalen存储数据长度
int main()
{

	Client_Class NewTask;
//WrongIP:
	/*scanf_s("%s", InputStr, DefBufSize);
	sockaddr_in addr = getSockAddr_in(InputStr, DefSvrPort);
	if (addr.sin_addr.S_un.S_addr == INADDR_NONE) { puts("Illegal IP Address"); goto WrongIP; }
	PrintIP(addr.sin_addr.S_un.S_addr);
    */
	FromsockaddrPrintIPandPort(NewTask.Connection_Infor.addr);
	NewTask.File_DataMode = NewTask.err = NewTask.Now_BlkNO = 0;
	NewTask.Connection_Infor.FunctionType = 1;
WrongFIle:
	scanf_s("%s", NewTask.Connection_Infor.FilePath, DefBufSize);
	CreateFilePointer(GetFileName(NewTask.Connection_Infor.FilePath),NewTask.Connection_Infor.FunctionType,NewTask.Connection_Infor.Local_FilePointer);

	NewTask.Download_File();
	//*/
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