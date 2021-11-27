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
	char YorN;
WrongReadPrametersMode:
	puts("If you wish to input parameters from Configuration.txt ?(Y/y/1 or N/n/0)");
	YorN=getchar();
	if (YorN == 'Y' || YorN == 'y' || YorN == '1')
	{
		ReadInforFromConfiguration = 1;
	}
	else if (YorN == 'N' || YorN == 'n' || YorN == '0') {
		ReadInforFromConfiguration = 0;
	}
	else {
		puts("Wrong Input Parameters Mode");
		goto WrongReadPrametersMode;
	}

	if (ReadInforFromConfiguration) freopen("Configuration.txt", "r", stdin);   //从Configuration.txt中读入输入 否则就使用控制台
	if (!ReadInforFromConfiguration) puts("Please Choose if show details(0 or 1):");
	scanf("%d", &NumofThread);
	showInfo = NumofThread ? 1 : 0;
	if (EchoInputPara) outt(showInfo),hh;
WrongThreadNum:
	if (!ReadInforFromConfiguration) puts("Please Input Number of Thread(no more than 100 and no less than 1):");
	scanf("%d", &NumofThread);
	if (EchoInputPara) outt(NumofThread),hh;
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

		if (!ReadInforFromConfiguration) printf("Please Input File DataMode(0->octet,1->netascii)(default octet) for thread %d:",i);
		scanf("%d", &NewTask[i].File_DataMode);
		if (EchoInputPara) outt(NewTask[i].File_DataMode),hh;
		//NewTask.File_DataMode = 1;
		NewTask[i].err = NewTask[i].Now_BlkNO = 0;

	WrongWorkMode:
		NewTask[i].Connection_Infor.FunctionType = -1;  //默认未定义的代码
		if (!ReadInforFromConfiguration) printf("Please Input Client Work Mode(1->Download ,2->Upload) for thread %d:",i);
		scanf("%d", &NewTask[i].Connection_Infor.FunctionType);
		if (NewTask[i].Connection_Infor.FunctionType != 1 && NewTask[i].Connection_Infor.FunctionType != 2) {
			puts("Wrong Work Mode");
			goto WrongWorkMode;
		}
		if (EchoInputPara) outt(NewTask[i].Connection_Infor.FunctionType),hh;

	WrongFile:
		strcpy(InputStr, ".\\Files\\");  //默认放置到这个文件里面，而服务器默认是NewTask.Connection_Infor.FilePath文件
		if (!ReadInforFromConfiguration) printf("Please Input File Path for thread %d (Local File will be inside .\Files\):",i);
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
	}
	puts("Press Enter to Start");
	system("pause");
	for(int i=1;i<=NumofThread;++i) Threads[i] = (HANDLE)_beginthreadex(NULL, 0, BeginMultiThread, (void*)(&NewTask[i]), 0, NULL);
	for (int i = 1; i <= NumofThread; ++i) if(Threads[i]!=NULL)WaitForSingleObject(Threads[i],INFINITE);  //等待所有正确建立的线程运行结束
	return 0;
}



int InputViaAnyOption(int argc, char* argv[]) {
	AnyOption* opt = new AnyOption();
	Client_Class NewTask;
	int NumofThread;
	//opt->setVerbose();   /* print warnings about unknown options */
	//opt->autoUsagePrint(true);   /* print usage for bad options */
	//opt->addUsage("Help: Socket_Programming [-t <Number of thread>] -i <serverip> -u|d <filepath> [-m <mode>] [-s]");
	opt->addUsage("Help: ");
	opt->addUsage("    If you wish to use multithreaded transmission ,please use the console method.");
	opt->addUsage("    In order to use this method please run the program without entering any parameters.And follow the instructions");
	opt->addUsage("");
	opt->addUsage("    Otherwise,please enter the parameters according to the following BNF");
	opt->addUsage("    Socket_Programming.exe - i <serverip> -u|d <filepath> [-m <mode>] [-s]");
	opt->addUsage("    -h  --help      Print usage information and exit");
	//opt->addUsage("    -t  --thread    Figure out how many thread you want to use.Default is 1,and no more than 100");
	opt->addUsage("    -i  --ip        Server ip address xxx.xxx.xxx.xxx xxx in [0,255] ");
	opt->addUsage("    -u  --upload    Upload file to server ,follows the file name");
	opt->addUsage("    -d  --download  Download file from server ,follows the file name");
	opt->addUsage("    -m  --mode      TFTP modes of transfer: octet(b) or netascii(t).Default is octet");
	opt->addUsage("    -s  --show      Show detail about packet Default is false");
	opt->addUsage("");

	opt->setFlag("help", 'h');
	//opt->setOption("thread", 't');
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

	if (opt->getValue('i') || opt->getValue("ip")) {
		NewTask.Connection_Infor.addr.sin_family = AF_INET;
		NewTask.Connection_Infor.addr.sin_port = htons(DefPort);
		//adddr.sin_addr.S_un.S_addr = inet_addr(ip); 
		if (opt->getValue('i')) NewTask.err = inet_pton(AF_INET, opt->getValue('i'), (void*)(&NewTask.Connection_Infor.addr.sin_addr.S_un.S_addr));
		else  NewTask.err = inet_pton(AF_INET, opt->getValue("ip"), (void*)(&NewTask.Connection_Infor.addr.sin_addr.S_un.S_addr));
		if (NewTask.err != 1) NewTask.Connection_Infor.addr.sin_addr.S_un.S_addr = DefIp;
	}
	else {
		puts("Error ip");
		opt->printUsage();
		return 0;
	}
		if (EchoInputPara) FromsockaddrPrintIPandPort(NewTask.Connection_Infor.addr);

	NewTask.err = NewTask.Now_BlkNO = 0;
	NewTask.Connection_Infor.FunctionType = -1;  //默认未定义的代码

	if (opt->getValue('m') || opt->getValue("mode")) {
		if (opt->getValue('m')) {
			strcpy(InputStr, opt->getValue('m'));
		}
		else strcpy(InputStr, opt->getValue("mode"));
		if (!strcmp(InputStr, "t")) strcpy(InputStr, RQMode[1]);
		else if (!strcmp(InputStr, "b")) strcpy(InputStr, RQMode[0]);
		if (strcmp(InputStr, RQMode[1]) && strcmp(InputStr, RQMode[0])) {
			puts("Error Mode");
			opt->printUsage();
			return 0;
		}
	}
	else strcpy(InputStr, RQMode[0]);
	NewTask.File_DataMode = strcmp(InputStr, RQMode[1]) ? 0 : 1;
		if (EchoInputPara) outt(NewTask.File_DataMode), hh;

	strcpy(InputStr, ".\\Files\\");  //默认放置到这个文件里面，而服务器默认是NewTask.Connection_Infor.FilePath文件
	if ((opt->getValue('u') || opt->getValue("upload"))
		&& (opt->getValue('d') || opt->getValue("download"))) {
		puts("Conflicting Work Type");
		opt->printUsage();
		return 0;
	}
	else if (opt->getValue('u') || opt->getValue("upload")) {
		NewTask.Connection_Infor.FunctionType=2;
		if (opt->getValue('u')) strcpy(NewTask.Connection_Infor.FilePath, opt->getValue('u'));
		else strcpy(NewTask.Connection_Infor.FilePath, opt->getValue("upload"));
	}
	else if (opt->getValue('d') || opt->getValue("download")) {
		NewTask.Connection_Infor.FunctionType = 1;
		if (opt->getValue('d')) strcpy(NewTask.Connection_Infor.FilePath, opt->getValue('d'));
		else strcpy(NewTask.Connection_Infor.FilePath, opt->getValue("download"));
	}
	else {
		puts("Error Work Type");
		opt->printUsage();
		return 0;
	}
		if (EchoInputPara) outt(NewTask.Connection_Infor.FunctionType), hh;
		if (EchoInputPara) outt(NewTask.Connection_Infor.FilePath),hh;
	strcpy(InputStr + 8, NewTask.Connection_Infor.FilePath);
	NewTask.err = CreateFilePointer(InputStr, (((NewTask.Connection_Infor.FunctionType - 1) ? 1 : 0) | (NewTask.File_DataMode ? 2 : 0)), NewTask.Connection_Infor.Local_FilePointer);

	if (opt->getFlag('s') || opt->getFlag("show"))
	TFTP::showInfo = true;
		if (EchoInputPara) outt(TFTP::showInfo), hh;
	puts("Press Enter to Start");
	system("pause");
	if (NewTask.Connection_Infor.FunctionType == 1) {
		NewTask.Download_File();
		//_endthreadex
	}
	else if (NewTask.Connection_Infor.FunctionType == 2) {
		NewTask.Upload_File();
	}

	delete opt;
	return 0;
}


int main(int argc, char* argv[])
{
	ll Start_time = GetCurrentmsTime();
	switch (argc) {
		case 1:
			InputViaConsole();
			break;
		default:
			InputViaAnyOption(argc, argv);
			break;
	}
	//*/
	Log_Output::Log_Msg << "TOT Time used for The Whole Process: " << (double )(GetCurrentmsTime() - Start_time)/1000.0 << " s";
	Log_Output::OutputtoBoth(2, NULL);
	return 0;
}