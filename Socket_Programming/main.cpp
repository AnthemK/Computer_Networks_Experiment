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

int InputViaConsole() {
	Client_Class NewTask;
	if (ReadInforFromConfiguration) freopen("Configuration.txt", "r", stdin);   //从Configuration.txt中读入输入 否则就使用控制台
WrongIP:
	if(!ReadInforFromConfiguration) puts("Please Input IP:");
	scanf_s("%s", InputStr, DefBufSize);
		outt(InputStr); hh;
	NewTask.Connection_Infor.addr.sin_family = AF_INET;
	NewTask.Connection_Infor.addr.sin_port = htons(DefPort);
	//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
	NewTask.err = inet_pton(AF_INET, InputStr, (void*)(&NewTask.Connection_Infor.addr.sin_addr.S_un.S_addr));
	if (NewTask.err != 1) NewTask.Connection_Infor.addr.sin_addr.S_un.S_addr = DefIp;
	FromsockaddrPrintIPandPort(NewTask.Connection_Infor.addr);

	if (!ReadInforFromConfiguration) puts("Please Input File DataMode(0->octet,1->netascii):");
	scanf("%d", &NewTask.File_DataMode);
		outt(NewTask.File_DataMode); hh;
	//NewTask.File_DataMode = 1;
	NewTask.err = NewTask.Now_BlkNO = 0;

	NewTask.Connection_Infor.FunctionType = -1;  //默认未定义的代码
	if (!ReadInforFromConfiguration) puts("Please Input Client Work Mode(1->Download ,2->Upload):");
	scanf("%d", &NewTask.Connection_Infor.FunctionType);
		outt(NewTask.Connection_Infor.FunctionType); hh;
WrongFile:
	strcpy(InputStr, ".\\Files\\");  //默认放置到这个文件里面，而服务器默认是NewTask.Connection_Infor.FilePath文件
	if (!ReadInforFromConfiguration) puts("Please Input File Path:");
	scanf_s("%s", NewTask.Connection_Infor.FilePath, DefBufSize);
		outt(NewTask.Connection_Infor.FilePath); hh;
	strcpy(InputStr + 8, NewTask.Connection_Infor.FilePath);
	NewTask.err = CreateFilePointer(InputStr, (((NewTask.Connection_Infor.FunctionType - 1) ? 1 : 0) | (NewTask.File_DataMode ? 2 : 0)), NewTask.Connection_Infor.Local_FilePointer);//NewTask.Connection_Infor.FunctionType-1是为了压位
	if (NewTask.err) { Log_Output::OutputtoBoth(2, "Wrong FileName!!!");  goto WrongFile; }
	cout << "NewTask.Connection_Infor.FunctionType: " << NewTask.Connection_Infor.FunctionType << endl;
	if (NewTask.Connection_Infor.FunctionType == 1) {
		NewTask.Download_File();
	}
	else if (NewTask.Connection_Infor.FunctionType == 2) {
		NewTask.Upload_File();
	}
	else Log_Output::OutputtoBoth(2, "Wrong FunctionType!!!");
	return 0;
}

int main()
{
	InputViaConsole();  //通过Console执行程序

	//*/
	return 0;
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