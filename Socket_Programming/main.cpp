// Socket_Programming.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//#include "stdafx.h"
/*#include "Const.h"
#include "Log_Output.h"
#include "Packet.h"
#include "General_Class.h"
*/
#include "Client.h"
#include "anyoption.h"
#include "process.h"
using namespace std;
using namespace TFTP;
const int maxn = 10000;

char InputStr[maxn],SendStr[maxn];
int err, datalen;  //err存储函数返回信息， datalen存储数据长度
int NumofThread;  //存储运行的线程个数

unsigned int __stdcall BeginMultiThread(void* Para) {
	Client_Class* NowTask = (Client_Class *)Para;
	if (NowTask->Connection_Infor.FunctionType == 1) {
		NowTask->Download_File();
		//_endthreadex
	}
	else if (NowTask->Connection_Infor.FunctionType == 2) {
		NowTask->Upload_File();
	}
	else Log_Output::OutputtoBoth(2, "Wrong FunctionType!!!");
	_endthreadex(0);
	return 0;
}

int InputViaConsole() {
	Client_Class NewTask[105];
	HANDLE Threads[105];
	if (ReadInforFromConfiguration) freopen("Configuration.txt", "r", stdin);   //从Configuration.txt中读入输入 否则就使用控制台
	if (!ReadInforFromConfiguration) puts("Please Choose if show details:");
	scanf("%d", &NumofThread);
	showInfo = NumofThread ? 1 : 0;
	if (EchoInputPara) outt(showInfo);
WrongThreadNum:
	if (!ReadInforFromConfiguration) puts("Please Input Number of Thread(no more than 100):");
	scanf("%d", &NumofThread);
	if (EchoInputPara) outt(NumofThread);
	if (NumofThread < 1 || NumofThread>100) {goto WrongThreadNum;}


	for (int i = 1; i <= NumofThread; ++i)
	{
	WrongIP:
		if(!ReadInforFromConfiguration || EchoInputPara)printf("Params for thread %d:\n",i);

		if (!ReadInforFromConfiguration) printf("Please Input IP for thread %d:",i);
		scanf_s("%s", InputStr, DefBufSize);
		if (EchoInputPara) outt(InputStr),hh;
		NewTask[i].Connection_Infor.addr.sin_family = AF_INET;
		NewTask[i].Connection_Infor.addr.sin_port = htons(DefPort);
		//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
		NewTask[i].err = inet_pton(AF_INET, InputStr, (void*)(&NewTask[i].Connection_Infor.addr.sin_addr.S_un.S_addr));
		if (NewTask[i].err != 1) NewTask[i].Connection_Infor.addr.sin_addr.S_un.S_addr = DefIp;
		if (EchoInputPara) FromsockaddrPrintIPandPort(NewTask[i].Connection_Infor.addr);

		if (!ReadInforFromConfiguration) printf("Please Input File DataMode(0->octet,1->netascii) for thread %d:",i);
		scanf("%d", &NewTask[i].File_DataMode);
		if (EchoInputPara) outt(NewTask[i].File_DataMode),hh;
		//NewTask.File_DataMode = 1;
		NewTask[i].err = NewTask[i].Now_BlkNO = 0;

		NewTask[i].Connection_Infor.FunctionType = -1;  //默认未定义的代码
		if (!ReadInforFromConfiguration) printf("Please Input Client Work Mode(1->Download ,2->Upload) for thread %d:",i);
		scanf("%d", &NewTask[i].Connection_Infor.FunctionType);
		if (EchoInputPara) outt(NewTask[i].Connection_Infor.FunctionType),hh;
	WrongFile:
		strcpy(InputStr, ".\\Files\\");  //默认放置到这个文件里面，而服务器默认是NewTask.Connection_Infor.FilePath文件
		if (!ReadInforFromConfiguration) printf("Please Input File Path for thread %d:",i);
		scanf_s("%s", NewTask[i].Connection_Infor.FilePath, DefBufSize);
		if (EchoInputPara) outt(NewTask[i].Connection_Infor.FilePath),hh;
		strcpy(InputStr + 8, NewTask[i].Connection_Infor.FilePath);
		NewTask[i].err = CreateFilePointer(InputStr, (((NewTask[i].Connection_Infor.FunctionType - 1) ? 1 : 0) | (NewTask[i].File_DataMode ? 2 : 0)), NewTask[i].Connection_Infor.Local_FilePointer);//NewTask.Connection_Infor.FunctionType-1是为了压位
		if (NewTask[i].err) { Log_Output::OutputtoBoth(2, "Wrong FileName!!!");  goto WrongFile; }


		if (NumofThread == 1)
		{
			if (NewTask[i].Connection_Infor.FunctionType == 1) {
				NewTask[i].Download_File();
				//_endthreadex
			}
			else if (NewTask[i].Connection_Infor.FunctionType == 2) {
				NewTask[i].Upload_File();
			}
			else Log_Output::OutputtoBoth(2, "Wrong FunctionType!!!");
		}
		else Threads[i]=(HANDLE)_beginthreadex(NULL,0,BeginMultiThread,(void *)(&NewTask[i]),0,NULL);
	}

	for (int i = 1; i <= NumofThread; ++i) if(Threads[i]!=NULL)WaitForSingleObject(Threads[i],INFINITE);  //等待所有正确建立的线程运行结束
	return 0;
}



int InputViaAnyOption(int argc, char* argv[]) {
	AnyOption* opt = new AnyOption();
	Client_Class NewTask[105];
	int NumofThread;
	//opt->setVerbose();   /* print warnings about unknown options */
	//opt->autoUsagePrint(true);   /* print usage for bad options */
	opt->addUsage("Help: Socket_Programming [-t <Number of thread>]-i <serverip> -u|d <filepath> [-m <mode>] [-s]");
	opt->addUsage("");
	opt->addUsage("    -h  --help      Print usage information and exit");
	opt->addUsage("    -t  --thread    Figure out how many thread you want to use.Default is 1,and no more than 100");
	opt->addUsage("    -i  --ip        Server ip address xxx.xxx.xxx.xxx ,the number of parameter should fit the number of thread ");
	opt->addUsage("    -u  --upload    Upload file to server,the number of parameter should fit the number of thread ");
	opt->addUsage("    -d  --download  Download file from server,the number of parameter should fit the number of thread ");
	opt->addUsage("    -m  --mode      TFTP modes of transfer: octet(b) or netascii(t).Default is octet");
	opt->addUsage("    -s  --show      Show detail about packet");
	opt->addUsage("");

	opt->setFlag("help", 'h');
	opt->setOption("thread", 't');
	opt->setOption("ip", 'i');
	opt->setOption("upload", 'u');
	opt->setOption("download", 'd');
	opt->setOption("mode", 'm');
	opt->setFlag("show", 's');

	opt->processCommandArgs(argc, argv);
	if (!opt->hasOptions() || opt->getFlag('h') || opt->getFlag("help")) { /* print usage if no options */
		opt->printUsage();
		delete opt;
		return 0;
	}
	if (opt->getValue('t') || opt->getValue("thread")) {
		NumofThread = atoi(opt->getValue('t'));
		cout << "thread = " << opt->getValue('t') <<"              "<<NumofThread << endl;
	}
	else NumofThread = 1;
	NewTask[0].File_DataMode = 0;

	delete opt;
	return 0;
}




int main(int argc, char* argv[])
{
	ll Start_time = GetCurrentmsTime();
	InputViaConsole();  //通过Console执行程序
	//InputViaAnyOption(argc,argv);
	//*/
	Log_Output::Log_Msg << "TOT Time used for all thread: " << (double )(GetCurrentmsTime() - Start_time)/1000.0 << " ms";
	Log_Output::OutputtoBoth(2, NULL);
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